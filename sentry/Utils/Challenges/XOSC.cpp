#include "stdafx.h"

extern BYTE Keyvault;
extern BYTE Hypervisor;

typedef DWORD(*ExecuteSupervisorChallenge_t)(DWORD dwTaskParam1, PBYTE pbDaeTableName, DWORD cbDaeTableName, XOSC::Buffer::XOSC* pbBuffer, DWORD cbBuffer);
extern BOOL offlineFiles;

Xe XOSC {
	DWORD SpoofXOSChallenge(DWORD ExecuteSupervisorChallengeAddress, DWORD dwTaskParam1, PBYTE pbDaeTableName, DWORD cbDaeTableName, XOSC::Buffer::XOSC* pbBuffer, DWORD cbBuffer)
	{
		ZeroMemory(pbBuffer, cbBuffer);
	
		//TODO: everything
		if(offlineFiles){
			Tools::MemoryBuffer Mem;
			if(!Tools::CReadFile("XeO://XeOnline/HV.bin", Mem)) { Tools::XNotifyUI("sad", L"XeOnline - Challenge failed!\nCould not read file: HV.bin"); Wait(5); HalReturnToFirmware(HalFatalErrorRebootRoutine); }
			memcpy((PBYTE*)Hypervisor, Mem.GetData(), Mem.GetDataLength());
			Mem.ResetData();
			if(!Tools::CReadFile("XeO://KV.bin", Mem)) { Tools::XNotifyUI("sad", L"XeOnline - Challenge failed!\nCould not read file: KV.bin"); Wait(5); HalReturnToFirmware(HalFatalErrorRebootRoutine); }
			memcpy((PBYTE*)Keyvault, Mem.GetData(), Mem.GetDataLength());
			//Correct challenge

		}else{
			//Form Server Request
		}

		//Execute challenge normally
		ExecuteSupervisorChallenge_t ExecuteSupervisorChallenge = (ExecuteSupervisorChallenge_t)ExecuteSupervisorChallengeAddress;
		ExecuteSupervisorChallenge(dwTaskParam1, pbDaeTableName, cbDaeTableName, pbBuffer, cbBuffer);
	
		return ERROR_SUCCESS;
	}
}