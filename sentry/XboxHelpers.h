#pragma once

namespace sentry {

	enum SMC_TEMP_TYPE {
		SMC_TEMP_TYPE_CPU,
		SMC_TEMP_TYPE_GPU,
		SMC_TEMP_TYPE_MEMORY,
		SMC_TEMP_TYPE_CASE,
		SMC_TEMP_TYPE_COUNT
	};

	HRESULT GetSystemTemperatures(PFLOAT temperatureResult);

} // namespace sentry
