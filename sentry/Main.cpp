#include "stdafx.h"
#include "Utils/Utilities.h"
#include "SentryMessage.h"
#include "XboxHelpers.h"

namespace sentry {
	BOOL g_isDevkit = FALSE;
	BOOL g_terminating = FALSE;
	BOOL g_terminated = FALSE;

	in_addr g_xboxIP;

	PCHAR g_tempNames[] ={ "CPU", "GPU", "eDRAM", "Mobo" };
	FLOAT g_tempValues[SMC_TEMP_TYPE_COUNT] ={ 0.0f };
	FLOAT g_tempValuesCache[SMC_TEMP_TYPE_COUNT] ={ 0.0f };

	VOID TitleThread() {
		if (SUCCEEDED(GetXboxInternalIP(g_xboxIP))) {
			auto octets = g_xboxIP.S_un.S_un_b;
			SentryMessage("IP: %i.%i.%i.%i", octets.s_b1, octets.s_b2, octets.s_b3, octets.s_b4).Send();
		}

		static DWORD titleIdCache = 0;

		while (!g_terminating) {
			DWORD titleId = XamGetCurrentTitleId();
			if (titleIdCache != titleId) {
				titleIdCache = titleId;
				if (titleId != NULL) {
					PLDR_DATA_TABLE_ENTRY moduleHandle = (PLDR_DATA_TABLE_ENTRY)GetModuleHandle(NULL);
					SentryMessage("TitleID: %08x", titleId).Send();
				}
			}

			Sleep(60);
		}
	}

	VOID TempsThread() {
		const DWORD delay = 5000;
		static DWORD lastRun = 0;

		while (!g_terminating) {
			DWORD now = GetTickCount();
			if (lastRun + delay > now) {
				Sleep(60);
				continue;
			}

			lastRun = now;

			if (FAILED(GetSystemTemperatures(g_tempValues)))
				continue;

			SentryMessage msg;
			const float epsilon = 0.1f; // tolerance threshold of 0.1C; anything less is ignored

			for (auto i = 0; i < SMC_TEMP_TYPE_COUNT; i++) {
				if (utils::EqualWithTolerance(g_tempValues[i], g_tempValuesCache[i], epsilon))
					continue;

				g_tempValuesCache[i] = g_tempValues[i];
				msg.AppendLine("%s: %.1f", g_tempNames[i], g_tempValues[i]);
			}

			if (!msg.IsEmpty())
				msg.Send();
		}
	}

	VOID Init() {
		g_isDevkit =  *(DWORD*)0x8E038610 & 0x8000 ? FALSE : TRUE;
		utils::ThreadMe((LPTHREAD_START_ROUTINE)TitleThread, NULL);
		utils::ThreadMe((LPTHREAD_START_ROUTINE)TempsThread, NULL);
		SentryMessage().AppendLine().AppendLine("Sentry started!").Send();
	}
} // namespace sentry

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		if (XamLoaderGetDvdTrayState() != DVD_TRAY_STATE_OPEN) {
			sentry::utils::ThreadMe((LPTHREAD_START_ROUTINE)sentry::Init, NULL);
		}
	} else if (dwReason == DLL_PROCESS_DETACH) {
		sentry::g_terminating = TRUE;
		while (!sentry::g_terminated)
			Sleep(10);
	}
	return TRUE;
}
