#include "stdafx.h"

namespace Main {

	typedef enum _SMC_TEMPERATURE_TYPE {
		SMC_TEMP_TYPE_CPU,
		SMC_TEMP_TYPE_GPU,
		SMC_TEMP_TYPE_MEMORY,
		SMC_TEMP_TYPE_CASE,
		SMC_TEMP_TYPE_COUNT
	} SMC_TEMPERATURE_TYPE;

	BOOL Devkit = FALSE;
	DWORD LastTitleId = 0;
	BOOL Terminating = FALSE;
	FLOAT tempResults[SMC_TEMP_TYPE_COUNT] = { 0.0f, 0.0f, 0.0f, 0.0f };
	in_addr xboxip;

	HRESULT GetSystemTemperatures(PFLOAT temperatureResult){
		// Return an invalid argument if we dont have a valid array
		if(temperatureResult == NULL) return E_INVALIDARG;

		// Create some buffers to handle the input/output of our SMC calls
		BYTE smcMessage[16];
		BYTE smcResponse[16];

		// Initialize our buffers
		memset(smcMessage, 0, 16);
		memset(smcResponse, 0, 16);

		// Request our tempetures
		smcMessage[0] = 0x7;		// Temperature Request
		HalSendSMCMessage((LPVOID)smcMessage, (LPVOID)smcResponse);

		// Calculate our temperatures
		for( unsigned int typeIdx = 0; typeIdx < SMC_TEMP_TYPE_COUNT; typeIdx++){
			temperatureResult[typeIdx] = (FLOAT)((smcResponse[typeIdx * 2 + 1] | (smcResponse[typeIdx * 2 + 2] << 8 )) / 256.0f);
		}

		// Return Successfully
		return S_OK;
	}

	VOID TitleThread(){
		// blocking call
		HRESULT ipResult = Tools::GetXboxInternalIP(xboxip);

		if (SUCCEEDED(ipResult)) {
			auto octets = xboxip.S_un.S_un_b;
			printf("[Sentry] IP: %i.%i.%i.%i", octets.s_b1, octets.s_b2, octets.s_b3, octets.s_b4);
		}

		while (!Terminating){
			DWORD titleId = XamGetCurrentTitleId();
			if(LastTitleId != titleId){
				LastTitleId = titleId;
				PLDR_DATA_TABLE_ENTRY moduleHandle = (PLDR_DATA_TABLE_ENTRY)GetModuleHandle(0);
				printf("[Sentry] TitleID: %08x\n", titleId);
				//printf("[Sentry] ModuleHandle: %ls\n", moduleHandle->BaseDllName.Buffer);
			}
			Sleep(500);
		}
	}

	VOID TempThread() {
		while (!Terminating) {
			HRESULT tempReturn = GetSystemTemperatures(tempResults);
			if (tempReturn == S_OK) {
				printf("[Sentry] GPU: %.1f\n[Sentry] CPU: %.1f\n[Sentry] eDRAM: %.1f\n[Sentry] Mobo: %.1f\n", tempResults[SMC_TEMP_TYPE_GPU], tempResults[SMC_TEMP_TYPE_CPU], tempResults[SMC_TEMP_TYPE_MEMORY], tempResults[SMC_TEMP_TYPE_CASE]);
			}
			Sleep(10);
		}
	}

	VOID Init(){
		Devkit =  *(DWORD*)0x8E038610 & 0x8000 ? FALSE : TRUE;
		Tools::ThreadMe((LPTHREAD_START_ROUTINE)TitleThread, NULL);
		Tools::ThreadMe((LPTHREAD_START_ROUTINE)TempThread, NULL);
		printf("\nSentry started!\n");
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved){
	if(dwReason == DLL_PROCESS_ATTACH) {
		if(XamLoaderGetDvdTrayState() != DVD_TRAY_STATE_OPEN) {
			Tools::ThreadMe((LPTHREAD_START_ROUTINE)Main::Init, NULL);
		}
	} else if (dwReason == DLL_PROCESS_DETACH){
		Main::Terminating = TRUE;
		Sleep(2500);
	}	
	return TRUE;
}
