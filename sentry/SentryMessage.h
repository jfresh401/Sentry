#pragma once

class SentryMessage {
public:
	std::string Text;

	SentryMessage() { }
	SentryMessage(const std::string& text) : Text(text) { }

	void Send() { sendInternal(); }

protected:
	void sendInternal() {
		sendToDbgPrint(Text);
	}

private:
	void sendToDbgPrint(const std::string& text) {
		skprintf("%s", text.c_str());
	}
};
