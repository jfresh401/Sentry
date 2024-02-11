#include "stdafx.h"

Xe XAM {
	
	Xe Buffer {
		#pragma pack(1)
		typedef struct {
			XEKEYS_EXEC_HEADER header;
			BYTE bReserved1[8];
			WORD wHvMagic;
			WORD wHvVersion;
			WORD wHvQfe;
			WORD wBldrFlags;
			DWORD dwBaseKernelVersion;
			DWORD dwUpdateSequence;
			DWORD dwHvKeysStatusFlags;
			DWORD dwConsoleTypeSeqAllow;
			QWORD qwRTOC;
			QWORD qwHRMOR;
			BYTE bHvECCDigest[XECRYPT_SHA_DIGEST_SIZE];
			BYTE bCpuKeyDigest[XECRYPT_SHA_DIGEST_SIZE];
			BYTE bRandomData[0x80]; 
			WORD hvExAddr;
			BYTE bHvDigest[0x6];
		} XE_KEYS_BUFFER, *PXE_KEYS_BUFFER;
		C_ASSERT(sizeof(XE_KEYS_BUFFER) == 0x100);
		#pragma pack()
	}
	QWORD SpoofXamChallenge(VOID* pBuffer, DWORD dwFileSize, QWORD Input1, QWORD Input2, QWORD Input3, QWORD Input4);
}