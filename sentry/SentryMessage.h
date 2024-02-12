#pragma once

namespace sentry {

	class SentryMessage {
	public:
		std::string Text;

		SentryMessage() { }
		SentryMessage(const char* fmt, ...);
		SentryMessage(const std::string& str) : Text(str) { }

		~SentryMessage() { }

		SentryMessage& Append(const char* fmt, ...);
		SentryMessage& Append(const std::string& str) { Text += str; return *this; }

		SentryMessage& AppendLine() { Text += "\n"; return *this; }
		SentryMessage& AppendLine(const char* fmt, ...);
		SentryMessage& AppendLine(const std::string& str) { Text += str + "\n"; return *this; }

		bool IsEmpty() const { return Text.empty(); }

		void Send() { sendInternal(); }

	protected:
		void sendInternal() {
			if (Text.length() > 0 && Text[Text.length() - 1] != '\n')
				Text += "\n";
			sendToDbgPrint(Text);
		}

	private:
		void sendToDbgPrint(const std::string& text) {
			DbgPrint("[Sentry] %s", text.c_str());
		}
	};

} // namespace sentry
