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

} // namespace sentry
