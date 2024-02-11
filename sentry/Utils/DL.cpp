#include "stdafx.h"

typedef enum {
	DL_ORDINALS_LDAT = 1,
	DL_ORDINALS_STARTSYSMOD,
	DL_ORDINALS_SHUTDOWN,
	DL_ORDINALS_FORCEINILOAD,
	DL_ORDINALS_GETNUMOPTS,
	DL_ORDINALS_GETOPTINFO,
	DL_ORDINALS_GETOPTVAL,
	DL_ORDINALS_SETOPTVAL,
	DL_ORDINALS_GETOPTVALBYNAME,
	DL_ORDINALS_SETOPTVALBYNAME,
	DL_ORDINALS_GETDRIVELIST,
	DL_ORDINALS_GETDRIVEINFO,
	DL_ORDINALS_BOOTPARSEBUTTONS,
	DL_ORDINALS_PLUGINPATH,
} DL_ORDINALS;

typedef int (*DLAUNCHGETNUMOPTS)(int* totalOpts);
typedef int (*DLAUNCHGETOPTINFO)(int opt, PDWORD optType, PCHAR outStr, PDWORD currVal, PDWORD defValue, PDWORD optCategory);
typedef BOOL (*DLAUNCHGETOPTVAL)(int opt, PDWORD val);
typedef BOOL (*DLAUNCHSETOPTVAL)(int opt, PDWORD val);
typedef BOOL (*DLAUNCHGETOPTVALBYNAME)(char* optName, PDWORD val);
typedef BOOL (*DLAUNCHSETOPTVALBYNAME)(char* optName, PDWORD val);
typedef VOID (*DLAUNCHFORCEINILOAD)(PCHAR path);
typedef DWORD (*DLAUNCHSTARTSYSMODULE)(char* modPath);
typedef VOID (*DLAUNCHSHUTDOWN)(VOID);
typedef DWORD (*DLAUNCHGETDRIVELIST)(DWORD dev, PCHAR devDest, PCHAR mountName, PCHAR friendlyName);
typedef DWORD (*DLAUNCHGETDRIVEINFO)(PDWORD maxIniDrives, PDWORD maxDevLen);

extern BOOL IsDevkit;

static pldata ldat;
static DLAUNCHSTARTSYSMODULE dlaunchStartSysModule;
static DLAUNCHSHUTDOWN dlaunchShutdown;
static DLAUNCHFORCEINILOAD dlaunchForceIniLoad;
static DLAUNCHGETNUMOPTS dlaunchGetNumOpts;
static DLAUNCHGETOPTINFO dlaunchGetOptInfo;
static DLAUNCHGETOPTVAL dlaunchGetOptVal;
static DLAUNCHSETOPTVAL dlaunchSetOptVal;
static DLAUNCHGETOPTVALBYNAME dlaunchGetOptValByName;
static DLAUNCHSETOPTVALBYNAME dlaunchSetOptValByName;
static DLAUNCHGETDRIVELIST dlaunchGetDriveList;
static DLAUNCHGETDRIVEINFO dlaunchGetDriveInfo;
static PPLUGIN_LOAD_PATH dlaunchPluginPath = 0;

static HRESULT initializeExports() {
	if ((ldat = (pldata)ResolveFunction("launch.xex", DL_ORDINALS_LDAT)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchStartSysModule = (DLAUNCHSTARTSYSMODULE)ResolveFunction("launch.xex", DL_ORDINALS_STARTSYSMOD)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchShutdown = (DLAUNCHSHUTDOWN)ResolveFunction("launch.xex", DL_ORDINALS_SHUTDOWN)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchForceIniLoad = (DLAUNCHFORCEINILOAD)ResolveFunction("launch.xex", DL_ORDINALS_FORCEINILOAD)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchGetNumOpts = (DLAUNCHGETNUMOPTS)ResolveFunction("launch.xex", DL_ORDINALS_GETNUMOPTS)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchGetOptInfo = (DLAUNCHGETOPTINFO)ResolveFunction("launch.xex", DL_ORDINALS_GETOPTINFO)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchGetOptVal = (DLAUNCHGETOPTVAL)ResolveFunction("launch.xex", DL_ORDINALS_GETOPTVAL)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchSetOptVal = (DLAUNCHSETOPTVAL)ResolveFunction("launch.xex", DL_ORDINALS_SETOPTVAL)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchGetOptValByName = (DLAUNCHGETOPTVALBYNAME)ResolveFunction("launch.xex", DL_ORDINALS_GETOPTVALBYNAME)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchSetOptValByName = (DLAUNCHSETOPTVALBYNAME)ResolveFunction("launch.xex", DL_ORDINALS_SETOPTVALBYNAME)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchGetDriveList = (DLAUNCHGETDRIVELIST)ResolveFunction("launch.xex", DL_ORDINALS_GETDRIVELIST)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchGetDriveInfo = (DLAUNCHGETDRIVEINFO)ResolveFunction("launch.xex", DL_ORDINALS_GETDRIVEINFO)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	if ((dlaunchPluginPath = (PPLUGIN_LOAD_PATH)ResolveFunction("launch.xex", DL_ORDINALS_PLUGINPATH)) == NULL) {
		return ERROR_INVALID_ORDINAL;
	}

	return S_OK;
}

static HRESULT initializePluginLoadPath() {
	HRESULT ret;

	if (dlaunchPluginPath->magic == PLUGIN_LOAD_PATH_MAGIC && dlaunchPluginPath->devicePath != NULL && dlaunchPluginPath->iniPath != NULL) {

		std::string launchDevice;
		std::string launchFile;

		launchDevice.assign(dlaunchPluginPath->devicePath);
		launchFile.assign(dlaunchPluginPath->iniPath);

		std::string link("\\System??\\Stealthy:");
		std::string directory(launchDevice);
		directory.append(launchFile.substr(1, launchFile.find_last_of('\\')));

		if (FAILED(ret = mountPath(directory, link))) {
			return ret;
		}
	}
	else
	{
		if (IsDevkit) {
			std::string link("\\System ? ? \\Stealthy:");
			std::string directory("\\Device\\Harddisk0\\Partition1\\DEVKIT\\Stealthy\\");
			if (FAILED(ret = mountPath(directory, link))) {
				return ret;
			}
		}
	}

	return S_OK;
}

HRESULT initializeDirectory() {
	HRESULT ret;

	if (FAILED(ret = initializeExports())) {
		return ret;
	}

	if (FAILED(ret = initializePluginLoadPath())) {
		return ret;
	}

	return ERROR_SUCCESS;
}

HRESULT setLiveBlock(bool enabled) {
	HRESULT ret;

	DWORD value = enabled ? 1 : 0;

	if (!dlaunchSetOptValByName("liveblock", &value)) {
		return E_FAIL;
	}

	if (!dlaunchSetOptValByName("livestrong", &value)) {
		return E_FAIL;
	}

	return ERROR_SUCCESS;
}