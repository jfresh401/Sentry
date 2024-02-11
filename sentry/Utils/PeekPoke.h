#pragma once
#include "stdafx.h"

Xe PeekPoke {
	typedef enum _PEEK_POKE_TYPE {
		PEEK_BYTE  = 0,
		PEEK_WORD  = 1,
		PEEK_DWORD = 2,
		PEEK_QWORD = 3,
		PEEK_BYTES = 4,
		POKE_BYTE  = 5,
		POKE_WORD  = 6,
		POKE_DWORD = 7,
		POKE_QWORD = 8,
		POKE_BYTES = 9,
		PEEK_SPR   = 10
	} PEEK_POKE_TYPE;

	HRESULT InitializeHvPeekPoke();

	BYTE HvPeekBYTE(QWORD Address);
	WORD HvPeekWORD(QWORD Address);
	DWORD HvPeekDWORD(QWORD Address);
	QWORD HvPeekQWORD(QWORD Address);
	HRESULT HvPeekBytes(QWORD Address, PVOID Buffer, DWORD Size);

	HRESULT HvPokeBYTE(QWORD Address, BYTE Value);
	HRESULT HvPokeWORD(QWORD Address, WORD Value);
	HRESULT HvPokeDWORD(QWORD Address, DWORD Value);
	HRESULT HvPokeQWORD(QWORD Address, QWORD Value);
	HRESULT HvPokeBytes(QWORD Address, const void* Buffer, DWORD Size);
	QWORD HvGetFuseLine(BYTE fuseIndex);
}