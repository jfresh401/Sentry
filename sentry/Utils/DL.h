#pragma once
#include "stdafx.h"

typedef enum {
	DL_OPT_TYPE_BOOL = 0,
	DL_OPT_TYPE_WORD,
	DL_OPT_TYPE_WORDREGION,
	DL_OPT_TYPE_WORDPORT,
	DL_OPT_TYPE_DWORD,
	DL_OPT_TYPE_DWORDTIME,
	DL_OPT_TYPE_MAX_ACCESS,
	DL_OPT_TYPE_PATH = DL_OPT_TYPE_MAX_ACCESS,
	DL_OPT_TYPE_PATHQLB,
	DL_OPT_TYPE_PATHPLUGIN,
} DL_OPT_TYPES;

typedef enum {
	OPT_MIN = 0,
	OPT_CAT_PATHS,
	OPT_CAT_BEHAVIOR,
	OPT_CAT_NETWORK,
	OPT_CAT_TIMERS,
	OPT_CAT_PLUGINS,
	OPT_CAT_EXTERNAL,
	OPT_CAT_MAX
} DL_OPT_CATEGORIES;

typedef struct {
	DWORD ID;
	DWORD ltype;
	char link[MAX_PATH];
	char dev[MAX_PATH];
	USHORT versionMaj;
	USHORT versionMin;
	USHORT targetKernel;
	USHORT svnVer;
	DWORD options;
	DWORD DebugRoutine;
	DWORD DebugStepPatch;
	PBYTE tempData;
	DWORD iniPathSel;
} ldata, *pldata;

typedef struct {
	char launchpath[MAX_PATH];
	DWORD flags;
	DWORD dev;
	DWORD rootDev;
} keydata, *pkeydata;

#define PLUGIN_LOAD_PATH_MAGIC 0x504C5041

typedef struct {
	DWORD       magic;
	const char* devicePath;
	const char* iniPath;
} PLUGIN_LOAD_PATH, *PPLUGIN_LOAD_PATH;

HRESULT initializeDirectory();
HRESULT setLiveBlock(bool enabled);