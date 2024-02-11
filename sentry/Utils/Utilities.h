#include "stdafx.h"

namespace Tools {
	VOID ThreadMe(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);
	VOID XNotifyUI(char* Type, PWCHAR pwszStringParam);
	DWORD ApplyPatches(PVOID buffer);
}