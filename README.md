ADDED 3/31/2025: Updated SentrySync4 files to include ESP32-C3 setup for small form factor. ALSO, PAY ATTENTION TO NOTES ABOUT FLASHING ESP32C3 and ESP32 Dev Board.

ADDED 3/16/2025: SentrySync4 files. Functionality for 2 seperate screens added. Sorted things a little. 
ADDED 3/13/2025: Both TFT and OLED setups for wireless SentrySync. 
ADDED 3/9/2025: SentrySync can now be remote and wireless running even on batteries!
ADDED 3/6/2025: SentrySync is now in Full Color! New files and diagram added.
ADDED 2/24/2025: SentrySync2.ino and SentrySync3.ino work with wifi. SentrySync3.ino adds Over-The-Air update capabilities for uploading a sketch to ESP32 wirelessly. Updated diagram. Thanks to ViolentLambs and Charly Gaber for helping out with this. Check out Charly's Fiver account. He does great work! 
<a href="https://www.fiverr.com/s/1qQEpKK">
  <img src="https://mms.businesswire.com/media/20190711005205/en/732296/5/FIV_Logo_Green-Circle.jpg?download=1&_gl=1*xlfouw*_gcl_au*NDY3MTMzMDY4LjE3NDA0MDgxNzM.*_ga*MjA3NTI1NzA3OS4xNzQwNDA4MTcz*_ga_ZQWF70T3FK*MTc0MDQwODE3Mi4xLjAuMTc0MDQwODE3My41OS4wLjA." alt="Charly's Fiver Profile" width="200">
</a>

# NOTES
- **SentrySync1.ino:** Original by Element. All functions do not work correctly for some reason.
- **SentrySync2.ino:** Edited by ViolentLamb and Charly Gaber. All functions work as intended in original.
- **SentrySync3.ino:** Edited by JFresh. This is for 0.96inch OLED and 0.96inch TFT .ino files. You'll also need to use the "Libraries" folder to find arduino library files required. Drag and drop to replace these files in your arduino IDE files. This version also includes OTA update capabilites for the ESP. NO WIRES NEEDED TO UPDATE THE ESP CODE.
- **SentrySync4.ino:** Edited by JFresh. This is the remote version of the above, plus added a larger screen size alternative. Same libraries as above need to be used. There are a few versions. Console only screen, Controller/remote only screen, and both console and controller screens. See files/folders for details.

# Sentry
Sentry is a project I (Element) started as a fun experiment to bring a status screen to the Xbox 360, inspired by the old LCD mods for the original Xbox. Currently, the project is unfinished however, it does have a handful of features in its current state, including:  

- **Temperature Monitoring**: Actively updates and displays the consoles current temperature readings.
- **Network Information**: Displays the console's IP address when connected via Wi-Fi or LAN. 
- **Title ID and Game Data**: Reports the active Title ID and its related game information.   
- **Debugging Mode**: Ability to switch into a uart mode to monitor all data transmitted over serial, useful for general Xbox 360 development and debugging.

All of this is done via a dashlaunch plugin that reports this information to the OLEDs controller

## Sub-Projects  

- **Sentry Plugin**: A DashLaunch plugin that handles all the reporting.  
- **Sentry Sample**: A sample XEX project to demonstrate mode switching.  
- **Sentry Sync**: The main controller project that parses the transmitted data and displays it on the OLED. 
  
I eventually lost interest and shelved the project, but this should make for a great starting point for anyone looking to build upon these ideas.  

### Demo Video:  

<a href="https://www.youtube.com/watch?v=jQ3l50-DQSE">
  <img src="https://img.youtube.com/vi/jQ3l50-DQSE/maxresdefault.jpg" alt="Sentry Demo" width="550">
</a>

## Parts Search List

- **OLED Display**: 2.42" 128x64 SSD1309 OLED Display  (any i2c OLED should work, refer to wiring)
- **ESP32-­WROOM-­32**: ESP-WROOM-32 Dev Board
- **SD Module**: Arduino compatible Micro SD card module

## Simple Wiring Diagram -- UPDATED

<img src="diagram.jpg" alt="Simple Wiring Diagram" width="600">

# SentrySync - Full Color

<img src="Sentry Sync/SentrySync3/SentrySync3_TFT/Sentry_Full_Color.jpg" alt="SentrySync - Full Color Wiring Diagram" width="1000">

# SentrySync - Remote
<img src="Sentry Sync/SentrySync4/SentrySync4 - Remote/SentrySync_Controller.jpg" alt="SentrySync - Remote Wiring Diagram" width="1000">
