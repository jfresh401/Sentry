// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma warning (disable:4482)
#pragma warning (disable:4172)

#define SDPRINTF_ENABLED             0
#define SKPRINTF_ENABLED             01

#include <xtl.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <time.h>
#include <map>
#include <xam.h>
#include <xbdm.h>
#include "../libraries/xkelib/xkelib.h"
#include <xui.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <math.h>

#include "Utils/StringUtils.h"

#if SDPRINTF_ENABLED
#define sdprintf_impl                   DbgPrint
#define sdprintf(fmt, ...)              sdprintf_impl("[Sentry Dbg] " fmt "\n", __VA_ARGS__)
#else
#define sdprintf(fmt, ...)              ((void)__noop)
#endif

#if SKPRINTF_ENABLED
#define skprintf_impl                   DbgPrint
#define skprintf(fmt, ...)              skprintf_impl("[Sentry] " fmt "\n", __VA_ARGS__)
#else
#define skprintf(fmt, ...)              ((void)__noop)
#endif

//game specific
