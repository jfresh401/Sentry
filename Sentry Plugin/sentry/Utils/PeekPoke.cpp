#include "stdafx.h"
#pragma warning(disable:4826)

namespace peekpoke {

	const DWORD HvPeekPokeExpID = 0x48565050;

	static QWORD _declspec(naked) HvxExpansionInstall(DWORD PhysicalAddress, DWORD CodeSize) {
		__asm {
			li			r0, 0x72
			sc
			blr
		}
	}

	static QWORD _declspec(naked) HvxExpansionCall(DWORD ExpansionId, QWORD Param1 = 0, QWORD Param2 = 0, QWORD Param3 = 0, QWORD Param4 = 0) {
		__asm {
			li			r0, 0x73
			sc
			blr
		}
	}

	static DWORD __declspec(naked) HvxExecute(DWORD key, DWORD mode, UINT64 dest, UINT64 src, UINT64 size, UINT64 r8) {
		__asm
		{
			li      r0, 0x0
			sc
			blr
		}
	}

	HRESULT InitializeHvPeekPoke() {
		VOID* pPhysExp = XPhysicalAlloc(0x1000, MAXULONG_PTR, 0, PAGE_READWRITE);
		DWORD physExpAdd = (DWORD)MmGetPhysicalAddress(pPhysExp);

		ZeroMemory(pPhysExp, 0x1000);
		memcpy(pPhysExp, HvPeekPokeExp, sizeof(HvPeekPokeExp));

		HRESULT result = (HRESULT)HvxExpansionInstall(physExpAdd, 0x1000);

		XPhysicalFree(pPhysExp);

		return result;
	}

	BYTE HvPeekBYTE(QWORD Address) { return (BYTE)HvxExpansionCall(HvPeekPokeExpID, PEEK_BYTE, Address); }
	WORD HvPeekWORD(QWORD Address) { return (WORD)HvxExpansionCall(HvPeekPokeExpID, PEEK_WORD, Address); }
	DWORD HvPeekDWORD(QWORD Address) { return (DWORD)HvxExpansionCall(HvPeekPokeExpID, PEEK_DWORD, Address); }
	QWORD HvPeekQWORD(QWORD Address) { return HvxExpansionCall(HvPeekPokeExpID, PEEK_QWORD, Address); }

	HRESULT HvPeekBytes(QWORD Address, PVOID Buffer, DWORD Size) {
		VOID* data = XPhysicalAlloc(Size, MAXULONG_PTR, 0, PAGE_READWRITE);
		ZeroMemory(data, Size);

		HRESULT result = (HRESULT)HvxExpansionCall(HvPeekPokeExpID, PEEK_BYTES, Address, (QWORD)MmGetPhysicalAddress(data), Size);

		if (result == S_OK) memcpy(Buffer, data, Size);

		XPhysicalFree(data);
		return result;
	}

	HRESULT HvPokeBYTE(QWORD Address, BYTE Value) { return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_BYTE, Address, Value); }
	HRESULT HvPokeWORD(QWORD Address, WORD Value) { return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_WORD, Address, Value); }
	HRESULT HvPokeDWORD(QWORD Address, DWORD Value) { return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_DWORD, Address, Value); }
	HRESULT HvPokeQWORD(QWORD Address, QWORD Value) { return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_QWORD, Address, Value); }

	HRESULT HvPokeBytes(QWORD Address, const void* Buffer, DWORD Size) {
		VOID* data = XPhysicalAlloc(Size, MAXULONG_PTR, 0, PAGE_READWRITE);
		memcpy(data, Buffer, Size);

		HRESULT result = (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_BYTES, Address, (QWORD)MmGetPhysicalAddress(data), Size);

		XPhysicalFree(data);
		return result;
	}

	QWORD HvGetFuseLine(BYTE fuseIndex) {
		if (fuseIndex > 11) return 0;
		return HvPeekQWORD(0x8000020000020000 + (fuseIndex * 0x200));
	}

} // namespace peekpoke
