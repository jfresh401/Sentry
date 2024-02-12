#include "stdafx.h"
#include "Utilities.h"

// TODO: wtf?
#pragma warning(push)
#pragma warning(disable:4826)

namespace sentry {
	namespace utils {

		VOID ThreadMe(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param = NULL) {
			HANDLE handle;
			DWORD lpThreadId;
			ExCreateThread(&handle, 0, &lpThreadId, (void*)XapiThreadStartup, lpStartAddress, param, 0x2 | CREATE_SUSPENDED);
			XSetThreadProcessor(handle, 4);
			SetThreadPriority(handle, THREAD_PRIORITY_ABOVE_NORMAL);
			ResumeThread(handle);
		}

	} // namespace utils
} // namespace sentry
