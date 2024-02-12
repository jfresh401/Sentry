#include "stdafx.h"

namespace Tools {
	VOID ThreadMe(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);
	VOID XNotifyUI(char* Type, PWCHAR pwszStringParam);
	DWORD ApplyPatches(PVOID buffer);

	HRESULT GetXboxInternalIP(in_addr& xboxAddress, int timeoutMs = 10000);
}
