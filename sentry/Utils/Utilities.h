#include "stdafx.h"

namespace sentry {
	namespace utils {

		VOID ThreadMe(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);
		HRESULT GetXboxInternalIP(in_addr& xboxAddress, int timeoutMs = 10000);

	} // namespace utils
} // namespace sentry
