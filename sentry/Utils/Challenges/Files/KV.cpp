#include "stdafx.h"

#define KvPtrDev 0x80000102000162e0
#define KvPtrRetail 0x80000102000163C0

extern KEY_VAULT keyVault;
extern BOOL IsDevkit;
extern BYTE cpuKey[0x10];
extern BYTE cpuKeyDigest[XECRYPT_SHA_DIGEST_SIZE];
extern BYTE kvDigest[XECRYPT_SHA_DIGEST_SIZE];
Xe KV {
	bool kvUseOddFlag = false;
	BYTE RetailRoamableObfuscationKey[0x10] = { 0xE1, 0xBC, 0x15, 0x9C, 0x73, 0xB1, 0xEA, 0xE9, 0xAB, 0x31, 0x70, 0xF3, 0xAD, 0x47, 0xEB, 0xF3 };
	BYTE ClientFreshKV[0x4000];
	DWORD kvCbFlag;
	DWORD kvHardwareFlag;
	QWORD kvPcieFlag;
	BYTE kvFuseKey[0x10];

	unsigned char coronaKey[16] = {
		0x1D, 0x79, 0x04, 0xD4, 0x1E, 0x28, 0x08, 0x6E, 0x02, 0x06, 0xE8, 0xAA,
		0xC7, 0xDC, 0xA6, 0xBC
	};

	unsigned char trinityKey[16] = {
		0xDB, 0xE6, 0x35, 0x87, 0x78, 0xCB, 0xFC, 0x2F, 0x52, 0xA3, 0xBA, 0xF8,
		0x92, 0x45, 0x8D, 0x65
	};

	unsigned char jasperKey[16] = {
		0xFF, 0x23, 0x99, 0x90, 0xED, 0x61, 0xD1, 0x54, 0xB2, 0x31, 0x35, 0x99,
		0x0D, 0x90, 0xBD, 0xBC
	};

	unsigned char falconKey[16] = {
		0x4E, 0xEA, 0xA3, 0x32, 0x3D, 0x9F, 0x40, 0xAA, 0x90, 0xC0, 0x0E, 0xFC,
		0x5A, 0xD5, 0xB0, 0x00
	};

	unsigned char xenonKey[16] = {
		0x52, 0x7A, 0x5A, 0x4B, 0xD8, 0xF5, 0x05, 0xBB, 0x94, 0x30, 0x5A, 0x17,
		0x79, 0x72, 0x9F, 0x3B
	};

	BYTE* getKvFuseKey(BYTE* kv)
	{
		BYTE fusekey[0x10];
		switch (*(DWORD*)(kv + 0x2245))
		{
			case XENON: memcpy(fusekey, xenonKey, 0x10); break;
			case ZEPHYR: memcpy(fusekey, xenonKey, 0x10); break;
			case FALCON: memcpy(fusekey, falconKey, 0x10); break;
			case JASPER: memcpy(fusekey, jasperKey, 0x10); break;
			case TRINITY: memcpy(fusekey, trinityKey, 0x10); break;
			case CORONA: memcpy(fusekey, coronaKey, 0x10); break;
			default: memcpy(fusekey, coronaKey, 0x10);
		}
		return fusekey;
	}

	void KvUseFlagTypeOdd(BYTE* kv)
	{
		if (*(WORD*)(kv + 0x1C) == ODD_POLICY_FLAG_CHECK_FIRMWARE) {
			kvUseOddFlag = true;
		} else {
			kvUseOddFlag = false;
		}
	}

	DWORD getKvCbFlag(BYTE* kv)
	{
		DWORD cbFlag;
		switch (*(DWORD*)(kv + 0x2245))
		{
			case XENON: cbFlag = 0x010B0524; break;
			case ZEPHYR: cbFlag = 0x010C0AD0; break;
			case FALCON: cbFlag = 0x010C0AD8; break;
			case JASPER: cbFlag = 0x010C0FFB; break;
			case TRINITY: cbFlag = 0x0304000D; break;
			case CORONA: cbFlag = 0x0304000E; break;
			default: cbFlag = 0x0304000E;
		}
		return cbFlag;
	}

	DWORD getKvHardwareFlag(BYTE* kv)
	{
		DWORD hadrwareFlag;
		switch (*(DWORD*)(kv + 0x2245))
		{
			case XENON: hadrwareFlag = 0x00000207; break;
			case ZEPHYR: hadrwareFlag = 0x10000207; break;
			case FALCON: hadrwareFlag = 0x20000207; break;
			case JASPER: hadrwareFlag = 0x30000207; break;
			case TRINITY: hadrwareFlag = 0x40000207; break;
			case CORONA: hadrwareFlag = 0x50000207; break;
			default: hadrwareFlag = 0x50000207;
		}
		return hadrwareFlag;
	}

	QWORD getKvPcieFlag(BYTE* kv)
	{
		QWORD pcieFlag;
		switch (*(DWORD*)(kv + 0x2245))
		{
			case XENON: pcieFlag = 0x2158023102000380; break;
			case ZEPHYR: pcieFlag = 0x2158023102000380; break;
			case FALCON: pcieFlag = 0x2158023102000380; break;
			case JASPER: pcieFlag = 0x3158116002000380; break;
			case TRINITY: pcieFlag = 0x4158016002000380; break;
			case CORONA: pcieFlag = 0x4158019002000380; break;
			default: pcieFlag = 0x4158019002000380;
		}
		return pcieFlag;
	}

	HRESULT SetKeyVault(BYTE* KeyVault)
	{
		memcpy(ClientFreshKV, KeyVault, 0x4000);
		memcpy(ClientFreshKV + 0x50, cpuKey, 0x10);
		memcpy(&keyVault, KeyVault, 0x4000);
		SetMemory((PVOID)0x8E03A000, &keyVault.ConsoleCertificate, 0x1A8);
		SetMemory((PVOID)0x8E038020, &keyVault.ConsoleCertificate.ConsoleId, 0x5);

		BYTE updateHash[0x14];
		XeCryptSha((BYTE*)0x8E038014, 0x3EC, NULL, NULL, NULL, NULL, updateHash, 0x14);
		SetMemory((PVOID)0x8E038000, updateHash, 0x14);

		BYTE sataUpdateHash[0x14];
		XeCryptSha((BYTE*)0x8E038794, 0x8C, 0, 0, 0, 0, sataUpdateHash, 0x14);
		SetMemory((PVOID)0x8E038794, sataUpdateHash, 0x14);

		KvUseFlagTypeOdd(KeyVault);
		kvCbFlag = getKvCbFlag(KeyVault);
		kvHardwareFlag = getKvHardwareFlag(KeyVault);
		kvPcieFlag = getKvPcieFlag(KeyVault);
		memcpy(kvFuseKey, getKvFuseKey(KeyVault), 0x10);
		if (kvCbFlag == 0 || kvHardwareFlag == 0 || kvPcieFlag == 0)
			return E_FAIL;

		memcpy(kvDigest, &keyVault.HmacShaDigest, 0x10);
		memcpy(kvDigest + 0x10, kvFuseKey, 0x4);
		SetMemory((PVOID)0x8E03AA30, cpuKeyDigest, 0x10);
		SetMemory((PVOID)0x8E03AA40, kvDigest, 0x10);
		SetMemory((PVOID)0x8E03AA50, kvFuseKey, 0x10);

		QWORD kvAddress = IsDevkit ? HvPeekQWORD(KvPtrDev) : HvPeekQWORD(KvPtrRetail);
		HvPeekBytes(kvAddress + 0xD0, &keyVault.ConsoleObfuscationKey, 0x40);
		memcpy(&keyVault.RoamableObfuscationKey, RetailRoamableObfuscationKey, 0x10);
	
			(kvAddress, &keyVault, 0x4000);
		return ERROR_SUCCESS;
	}

	HRESULT LoadKeyVault(CHAR* FilePath) {
		Tools::MemoryBuffer mbKv;
		if(!Tools::CReadFile(FilePath, mbKv)) {
			return E_FAIL;
		}
		return SetKeyVault(mbKv.GetData());
	}
}