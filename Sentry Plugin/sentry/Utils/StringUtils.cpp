#include "stdafx.h"
#include "StringUtils.h"

std::string strprintf(_In_z_ _Printf_format_string_ const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	auto result = vstrprintf(fmt, va);
	va_end(va);
	return result;
}

std::string vstrprintf(_In_z_ _Printf_format_string_ const char* fmt, va_list va)
{
	// int len = vsnprintf(nullptr, 0, fmt, va); //_vscprintf
	int         len = _vscprintf(fmt, va);
	std::string str;
	if (len >= 0) {
		str.resize(len + 1);
		// vsprintf_s(&str[0], len, fmt, va);
		vsnprintf(&str[0], len, fmt, va);
		str.resize(len);
	}
	return str;
}

std::wstring wstrprintf(_In_z_ _Printf_format_string_ const wchar_t* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	auto result = vwstrprintf(fmt, va);
	va_end(va);
	return result;
}

std::wstring vwstrprintf(_In_z_ _Printf_format_string_ const wchar_t* fmt, va_list va)
{
	// int len = vswprintf(nullptr, 0, fmt, va);
	int          len = _vscwprintf(fmt, va) + 1;
	std::wstring wstr;
	if (len >= 0) {
		wstr.resize(len + 1);
		// vswprintf_s(&wstr[0], len, fmt, va);
		vswprintf(&wstr[0], len, fmt, va);
		wstr.resize(len);
	}
	return wstr;
}

std::wstring StrtoWStr(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

std::string WStrtoStr(const std::wstring& wstr)
{
	return std::string(wstr.begin(), wstr.end()).assign(wstr.begin(), wstr.end());
}
