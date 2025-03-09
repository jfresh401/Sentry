#include <ArduinoOTA.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h> //agregado

#include <SD.h>
#include <SPI.h>

#include <esp_now.h>
 

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t receiverAddress[] = {0x5c, 0x01, 0x3b, 0x67, 0x58, 0x28};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  float gTemp;
  float cTemp;
  float eTemp;
  float mTemp;
  bool sd;
  bool uart;
  bool tidSent;
  char gameTitle[32];
  char developer[32];
  char tTitleID[10];
  char xboxipAdd[12];
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------


#define UART_BAUD_RATE 115200




const char* ssid = "Smiths";
const char* password = "capitol401";

WebServer server(80);

String consoleData = "";
bool autoScroll = true;

const int MAX_LINES_WEB = 200; // Maximum number of lines to save for web
const int MAX_LINES_SCREEN = 8; // Maximum number of lines to display on the screen
String lines_screen[MAX_LINES_SCREEN]; // Array to store lines of text
String lines_web[MAX_LINES_WEB];
int lineCountScreen = 0; // Counter to keep track of the number of lines displayed
int lineCountWeb = 0;
bool receivedLines = false;

int scrollSpeed = 0; // Speed of text scrolling, 0 being instant

bool uartMode = false; // Variable to track the current mode




unsigned long previousMillis = 0;
const long interval = 2000; // Interval to update OLED display (milliseconds)


//----------------------------------------------------------------------------------
//WiFi connection
boolean ConectWiFi(void){
  boolean state = true;
  byte c = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (c > 20){
      state = false;
      break;}
    c++;}
  return state;}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  
}
//----------------------------------------------------------------------------------
void setup() {
  Serial.begin(UART_BAUD_RATE);

  WiFi.mode(WIFI_STA);
 
  
  //Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  if(ConectWiFi()==true){
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());}
  else{
    Serial.println("");
    Serial.println("WiFi not connected.");}
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  delay(4000);
  //Start the web server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/serialdata", HTTP_GET, handleSerialData);
  server.on("/autoscroll", HTTP_GET, handleAutoScroll);
  server.on("/clear", HTTP_GET, handleClear);
  server.begin();
  Serial.println("HTTP server started");


  // Initialize SD card
  if (!SD.begin()) {
    Serial.println("SD Mount Failed");
    myData.sd = false;

  }
  else { 
    myData.sd = true;
  }
    //delay(3000); // Wait for 5 seconds
    
    
  //drawBackground();
  //delay(5000);
  //tft.fillScreen(ST7735_BLACK);
  ArduinoOTA.begin();
  Serial.println("OTA started");
}
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
void addLineWeb(String line) { 
  // Skip adding lines containing temp data so we aren't spamming the screen
  // if (line.indexOf("eDRAM:") != -1 || line.indexOf("GPU:") != -1 || line.indexOf("CPU:") != -1 || line.indexOf("Mobo:") != -1) {
  //   return;
  // }
  
  // If the web buffer is full, remove the oldest entry
  if (lineCountWeb >= MAX_LINES_WEB) {
    for (int i = 1; i < MAX_LINES_WEB; i++) {
      lines_web[i - 1] = lines_web[i];
    }
    lineCountWeb = MAX_LINES_WEB - 1;
  }

  // Add the new line at the bottom
  lines_web[lineCountWeb++] = line;

  receivedLines = true;
}

//----------------------------------------------------------------------------------
void loop() {
  ArduinoOTA.handle();
  
  server.handleClient();
  delay(10);//allow the cpu to switch to other tasks

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval && !uartMode) {
    // Time to update OLED display if not in UART mode
    previousMillis = currentMillis;
    // Send message via ESP-NOW
      
  }

  if (Serial.available() > 0) {
    // Read the incoming byte from UART
    String data = Serial.readStringUntil('\n');
    if (data.startsWith("[Sentry] UART Mode")) {
      uartMode = true; // Switch to UART mode
      myData.uart = true;
    } else if (data.startsWith("[Sentry] Default Mode")) {
      uartMode = false; // Switch to Default mode
      myData.uart = false;
      
    } else if (!uartMode) { // Process data only in Default mode
      processDefaultModeData(data);
      server.handleClient(); // Handle client requests
    }
    
    addLineWeb(data);
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  
    
    // Print the incoming data to the serial monitor
    Serial.println(data);
  }
}

//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
void processDefaultModeData(String data) {
  if (data.startsWith("[Sentry] GPU:")) {
    myData.gTemp = data.substring(13).toFloat();
  } else if (data.startsWith("[Sentry] CPU:")) {
    myData.cTemp = data.substring(13).toFloat();
  } else if (data.startsWith("[Sentry] eDRAM:")) {
    myData.eTemp = data.substring(15).toFloat();
  } else if (data.startsWith("[Sentry] Mobo:")) {
    myData.mTemp = data.substring(14).toFloat();
  } else if (data.startsWith("[Sentry] TitleID:")) {
    // Parse the title ID
    String titleID = data.substring(17);
    titleID.trim(); // Trim leading and trailing spaces
    if (titleID != "00000000") { // No idea why that get's reported but we ignore it
      // Display information from SD card
      displayInfoFromSD(titleID);
      
      
      
    }
  } else if (data.startsWith("[Sentry] IP:")) { // Check if the data contains IP address
    // Extract the IP address
    int ipStartIndex = data.indexOf(":") + 2; // Start index of the IP address
    String ipAddress = data.substring(ipStartIndex);

    // Check if the console has assigned an IP
    if (ipAddress != "0.0.0.0") {
      strcpy(myData.xboxipAdd, ipAddress.c_str());
      
    }
  }
}

//----------------------------------------------------------------------------------
void handleRoot() {
    String page = "<!DOCTYPE html>";
    page += "<html>";
    page += "<head>";
    page += "<title>Sentry Sync Webserver</title>";
    page += "<style> ";
    page += "body { ";
    page += "background-color: #212121; "; // Set background color
    page += "color: #B4B4B4; "; // Set text color
    page += "} ";
    page += "#console { ";
    page += "border: 1px solid black; ";
    page += "padding: 10px; ";
    page += "overflow-y: scroll; ";
    page += "height: 150px; ";
    page += "width: 35%; ";
    page += "} ";
    page += "#controls { ";
    page += "margin-top: 10px; ";
    page += "} ";
    page += "#controls label, #controls button { ";
    page += "display: inline-block; ";
    page += "margin-right: 10px; ";
    page += "} ";
    page += "</style>";
    page += "</head>";
    page += "<body>";
    page += "<h1>Sentry Sync Server For Xbox 360</h1>";
    page += "<h2>UART Output:</h2>";
    page += "<div id=\"console\">";
    page += "</div>";
    page += "<div id=\"controls\">";
    page += "<label><input type=\"checkbox\" id=\"autoscrollCheckbox\" onchange=\"toggleAutoScroll()\" checked> Autoscroll</label>";
    page += "<button onclick=\"clearConsole()\">Clear Output</button>";
    page += "</div>";
    page += "<script>";
    page += "var consoleDiv = document.getElementById('console');";
    page += "var autoScrollCheckbox = document.getElementById('autoscrollCheckbox');";
    page += "function updateConsole() {";
    page += "var xhr = new XMLHttpRequest();";
    page += "xhr.onreadystatechange = function() {";
    page += "if (xhr.readyState == 4 && xhr.status == 200) {";
    page += "var newData = xhr.responseText;";
    page += "if(newData !== 'No data available') {"; // Check if new data is available
    page += "consoleDiv.innerHTML += newData + '<br>';"; // Append new data with line break
    page += "if (autoScrollCheckbox.checked) {";
    page += "consoleDiv.scrollTop = consoleDiv.scrollHeight;"; // Scroll to bottom
    page += "}";
    page += "}";
    page += "}";
    page += "};";
    page += "xhr.open('GET', '/serialdata', true);";
    page += "xhr.send();";
    page += "}";
    page += "updateConsole();"; // Initially update console
    page += "setInterval(updateConsole, 100);"; // Update console every 100 milliseconds
    page += "function toggleAutoScroll() {";
    page += "autoScroll = autoScrollCheckbox.checked;";
    page += "}";
    page += "function clearConsole() {";
    page += "consoleDiv.innerHTML = '';"; // Clear console content
    page += "}";
    page += "</script>";
    page += "</body>";
    page += "</html>";

    server.send(200, "text/html", page); // Send response to the client
}

//----------------------------------------------------------------------------------
void handleSerialData() {
    String message = "";
    if(receivedLines){
      for (int i = 0; i < lineCountWeb; i++) {
        message += lines_web[i] + '\n';
        lines_web[i] = "";
      }
      lineCountWeb = 0;
      receivedLines = false;
    }
    server.send(200, "text/plain", message); // Send response to the client
}

//----------------------------------------------------------------------------------
void handleAutoScroll() {
    autoScroll = !autoScroll;
    server.send(200, "text/plain", String(autoScroll));
}

//----------------------------------------------------------------------------------
void handleClear() {
    consoleData = ""; // Clear console data
    server.send(200, "text/plain", "Console cleared");
}

//----------------------------------------------------------------------------------
void displayInfoFromSD(String targetTitleID) {
  targetTitleID.trim(); // Trim leading and trailing spaces

  // Get the elapsed time since the program started
  unsigned long elapsedSeconds = millis() / 1000;

  // Check if the elapsed time is less than 25 seconds and the target title ID is fffe07d1 (Dashboard)
  if (elapsedSeconds < 25 && targetTitleID.equalsIgnoreCase("fffe07d1")) {
    // Do nothing and return without processing
    return;
  }

  // Attempt to open the file
  File file = SD.open("/gamelist.txt");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Convert targetTitleID to lowercase
  targetTitleID.toLowerCase();

  // Read each line of the file
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim(); // Trim leading and trailing spaces

    // Extract the title ID from the line
    int asteriskIndex = line.indexOf('*');
    if (asteriskIndex == -1) {
      // Invalid line format, skip to the next line
      continue;
    }
    String titleID = line.substring(0, asteriskIndex);
    
    // Check if the extracted title ID matches the target title ID in lowercase
    if (titleID.equalsIgnoreCase(targetTitleID)) {
      // Parse information from the line
      String gameInfo = line.substring(asteriskIndex + 1);
      int asterisk1 = gameInfo.indexOf('*');
      int asterisk2 = gameInfo.indexOf('*', asterisk1 + 1);
      strcpy(myData.gameTitle, gameInfo.substring(0, asterisk1).c_str());
      strcpy(myData.developer, gameInfo.substring(asterisk1 + 1, asterisk2).c_str());

      targetTitleID.toUpperCase(); // Converts title id to uppercase
      strcpy(myData.tTitleID, targetTitleID.c_str()); 
      myData.tidSent == true;
      
      file.close();
      return; // Stop searching after finding the first match
    }
  }

  // Close the file
  file.close();

  Serial.println("Title ID not found");
}
