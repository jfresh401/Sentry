#pragma once
#include "stdafx.h"

namespace sentry {
	namespace utils {

		VOID ThreadMe(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);

		inline bool EqualWithTolerance(float a, float b, float minDeviation = 0.0001f) {
			return (fabsf(a - b) <= minDeviation);
		}

	} // namespace utils
} // namespace sentry
