#include "stdafx.h"

// Convenience macro to call a function pointer with a variable number of arguments
#define call(addr, ...) ((DWORD(*)(...))(DWORD)addr)(__VA_ARGS__)

const DWORD DbgPrintAdr = 0x81A72BC4;
const DWORD XamLoaderLaunchTitleExAddr = 0x816A0D70;
const DWORD XamLoaderRebootToDashAddr = 0x816A1330;

enum SENTRY_CMD {
	NONE,
	MODE,
	//... additional commands begin here
}

enum SENTRY_MODE_TYPE {
	NONE,
	DEFAULT
	UART,
	//... additional modes begin here
};

// Sends a string to DbgPrint with the specified mode.
VOID SetMode(SENTRY_MODE_TYPE mode) {
	// The arduino recognizes commands as any "[Sentry]" strings that begin with a '$' as a command
	// (in this case, "$mode" triggers a mode change to the specified value).
	call(DbgPrintAdr, "[Sentry] $mode=%d\n", mode); // [Sentry] $mode=1
}

VOID __cdecl main() {
	SetMode(SENTRY_MODE_TYPE::DEFAULT);

	call(XamLoaderLaunchTitleExAddr, "\\SystemRoot\\dash.xex", "\\SystemRoot", NULL, NULL);
	call(XamLoaderRebootToDashAddr, NULL);
}
