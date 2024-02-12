#pragma once

std::string  strprintf(_In_z_ _Printf_format_string_ const char* fmt, ...);
std::string  vstrprintf(_In_z_ _Printf_format_string_ const char* fmt, va_list va);
std::wstring wstrprintf(_In_z_ _Printf_format_string_ const wchar_t* fmt, ...);
std::wstring vwstrprintf(_In_z_ _Printf_format_string_ const wchar_t* fmt, va_list va);

std::wstring StrtoWStr(const std::string& str);
std::string  WStrtoStr(const std::wstring& wstr);
