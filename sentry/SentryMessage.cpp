#include "stdafx.h"
#include "SentryMessage.h"

SentryMessage::SentryMessage(const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	auto result = vstrprintf(fmt, va);
	va_end(va);
	Text = result;
}

SentryMessage& SentryMessage::Append(const char * fmt, ...) {
	va_list va;
	va_start(va, fmt);
	auto result = vstrprintf(fmt, va);
	va_end(va);
	Text += result;
	return *this;
}

SentryMessage& SentryMessage::AppendLine(const char * fmt, ...) {
	va_list va;
	va_start(va, fmt);
	auto result = vstrprintf(fmt, va);
	va_end(va);
	Text += result + "\n";
	return *this;
}
