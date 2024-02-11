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
}