#include "stdafx.h"

Xe XOSC {	
	Xe Buffer {
		#pragma pack(1)
		typedef struct _XOSC {
				DWORD                   dwResult;                     // 0x00-0x04
				WORD                    xoscMajor;                    // 0x04-0x06
				WORD                    xoscMinor;                    // 0x06-0x08
				QWORD                   qwOperations;                 // 0x08-0x10
				DWORD                   dvd_ioctl_res;                // 0x10-0x14
				DWORD                   xekeysgetkey_res;             // 0x14-0x18
				DWORD                   dwExecutionResult;            // 0x18-0x1C
				DWORD                   console_id_null;              // 0x1C-0x20
				DWORD					unk_hash_res;				  // 0x20-0x34
				DWORD					dae_result; 				  // 0x34-0x38
				XEX_EXECUTION_ID        xexExecutionId;               // 0x38-0x50
				BYTE                    cpuKeyHash[0x10];             // 0x50-0x60
				BYTE                    kvHash[0x10];                 // 0x60-0x70
				BYTE                    sec_fuses[0x10];              // 0x70-0x80
				DWORD                   drivePhaseLevel;              // 0x80-0x84
				DWORD                   titleID;		              // 0x84-0x8C
				BYTE                    unk_data1[0x64];              // 0x8C-0xF0
				BYTE                    driveData1[0x24];			  // 0xF0-0x114
				BYTE                    driveData2[0x24];			  // 0x114-0x138
				BYTE                    consoleSerial[0xC];           // 0x138-0x144
				WORD                    serial_byte;                  // 0x144-0x146
				WORD                    bldr_flags;                   // 0x146-0x148
				WORD                    xamRegion;                    // 0x148-0x14A
				WORD                    xamOdd;                       // 0x14A-0x14C
				DWORD                   beta_bldr;                    // 0x14C-0x150
				DWORD                   policy_flash;                 // 0x150-0x154
				DWORD                   xosc_region;                  // 0x154-0x158
				QWORD                   hv_keys_status_flags;         // 0x158-0x160
				BYTE					unk_data2[0x10];			  // 0x160-0x170
				QWORD					xosc_dae;					  // 0x170-0x178
				BYTE					unk_data3[0x8];				  // 0x178-0x180
				QWORD					kv_restricted_priv;			  // 0x180-0x188
				BYTE					unk_data4[0x10];			  // 0x188-0x198
				QWORD					hv_protected_flags;			  // 0x198-0x1A0
				BYTE					console_id[0x5];			  // 0x1A0-0x1A5
				BYTE					nulled_console_data[0x2b];    // 0x1A5-0x1D0
				DWORD					hardware_flags;				  // 0x1D0-0x1D4
				BYTE					nulled_hardware_flags[0xD4];  // 0x1D4-0x2A8
				DWORD					sizeMu0;					  // 0x2A8-0x2AC
				DWORD					sizeMu1;					  // 0x2AC-0x2B0
				DWORD					sizeMuSfc;					  // 0x2B0-0x2B4
				DWORD					sizeMuUsb;					  // 0x2B4-0x2B8
				DWORD					sizeExUsb0;					  // 0x2B8-0x2BC
				DWORD					sizeExUsb1;					  // 0x2BC-0x2C0
				DWORD					sizeExUsb2;					  // 0x2C0-0x2C4
				DWORD					clr_version;				  // 0x2C4-0x2C8
				BYTE					unk_data5[0x10];			  // 0x2C8-0x2D8
				DWORD					xosc_footer;				  // 0x2D8-0x2DC
				BYTE					unk_data6[0x4];				  // 0x2DC-0x2E0
				BYTE					unused_filler[0x120];		  // 0x2E0-0x400
		} XOSC, *pXOSC;
		#pragma pack()
	}
	DWORD SpoofXOSChallenge(DWORD ExecuteSupervisorChallengeAddress, DWORD dwTaskParam1, PBYTE pbDaeTableName, DWORD cbDaeTableName, XOSC::Buffer::XOSC* pbBuffer, DWORD cbBuffer);
}