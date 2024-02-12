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

		HRESULT GetXboxInternalIP(in_addr& xboxAddress, int timeoutMs) {
			DWORD timeout = GetTickCount() + timeoutMs;

			DWORD ethernet_status = XNetGetEthernetLinkStatus();
			if (!ethernet_status) {
				skprintf("Ethernet unplugged...");
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

	} // namespace utils
} // namespace sentry
