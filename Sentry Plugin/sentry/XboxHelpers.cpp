#include "stdafx.h"
#include "XboxHelpers.h"

namespace sentry {

	HRESULT GetSystemTemperatures(PFLOAT temperatureResult) {
		// Return error if we dont have a valid pointer for the results
		if (temperatureResult == nullptr)
			return E_INVALIDARG;

		// Create and initialize our SMC I/O buffers
		BYTE smcMessage[16] ={ 0x00 };
		BYTE smcResponse[16] ={ 0x00 };

		// Request our temperatures
		smcMessage[0] = smc_query_sensor;
		HalSendSMCMessage((LPVOID)smcMessage, (LPVOID)smcResponse);

		// Calculate our temperatures
		for (auto typeIdx = 0; typeIdx < SMC_TEMP_TYPE_COUNT; typeIdx++)
			temperatureResult[typeIdx] = (FLOAT)((smcResponse[typeIdx * 2 + 1] | (smcResponse[typeIdx * 2 + 2] << 8)) / 256.0f);

		return ERROR_SUCCESS;
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

} // namespace sentry
