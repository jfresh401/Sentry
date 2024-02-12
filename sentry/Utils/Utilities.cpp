#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:4826)

namespace Tools {

	VOID ThreadMe(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param = NULL) {
		HANDLE handle;
		DWORD lpThreadId;
		ExCreateThread(&handle, 0, &lpThreadId, (void*)XapiThreadStartup, lpStartAddress, param, 0x2 | CREATE_SUSPENDED);
		XSetThreadProcessor(handle, 4);
		SetThreadPriority(handle, THREAD_PRIORITY_ABOVE_NORMAL);
		ResumeThread(handle);
	}

	VOID XNotifyUI(char* Type, PWCHAR pwszStringParam) {
		XNOTIFYQUEUEUI_TYPE eType;
		if (strcmp(Type, "happy") == 0)       eType = XNOTIFYUI_TYPE_PREFERRED_REVIEW;
		else if (strcmp(Type, "sad") == 0)    eType = XNOTIFYUI_TYPE_AVOID_REVIEW;
		else if (strcmp(Type, "hammer") == 0) eType = XNOTIFYUI_TYPE_COMPLAINT;
		else if (strcmp(Type, "xbox") == 0)   eType = XNOTIFYUI_TYPE_CONSOLEMESSAGE;
		else if (strcmp(Type, "mail") == 0)   eType = XNOTIFYUI_TYPE_MAILBOX;
		else /*Unsupported Type*/			  eType = XNOTIFYUI_TYPE_PREFERRED_REVIEW;

		XNotifyQueueUI(eType, XUSER_INDEX_ANY, XNOTIFYUI_PRIORITY_HIGH, pwszStringParam, NULL);
	}

	DWORD ApplyPatches(PVOID buffer) {
		DWORD PatchCount = NULL;
		PDWORD PatchData = (PDWORD)buffer;
		while (*PatchData != 0xFFFFFFFF) {
			memcpy((PVOID)PatchData[0], &PatchData[2], PatchData[1] * sizeof(DWORD));
			PatchData += (PatchData[1] + 2);
			PatchCount++;
		}
		return PatchCount;
	}

	HRESULT GetXboxInternalIP(in_addr& xboxAddress, int timeoutMs) {
		DWORD timeout = GetTickCount() + timeoutMs;

		DWORD ethernet_status = XNetGetEthernetLinkStatus();
		if (!ethernet_status) {
			sdprintf("Ethernet unplugged...");
			while (!(ethernet_status = XNetGetEthernetLinkStatus())) {
				if (GetTickCount() > timeout)
					return E_FAIL;
				Sleep(50);
			}
		}

		timeout = GetTickCount() + timeoutMs;

		XNADDR xnaddr;
		while (XNetGetTitleXnAddr(&xnaddr) == XNET_GET_XNADDR_PENDING && GetTickCount() < timeout)
			Sleep(50);

		while ((!xnaddr.ina.S_un.S_addr || ((xnaddr.ina.S_un.S_addr & 0xFFFF0000) == 0xA9FE0000)) && GetTickCount() < timeout) {
			XNetGetTitleXnAddr(&xnaddr);
			Sleep(50);
		}

		xboxAddress = xnaddr.ina;

		if (!xnaddr.ina.S_un.S_addr)
			return E_FAIL;

		return ERROR_SUCCESS;
	}
}
