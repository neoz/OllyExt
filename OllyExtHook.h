#pragma once


typedef struct
{
	void* remoteTargetProc;
	DWORD trampolineProcSize;
	void* trampolineProc;
	DWORD hookSize;
	void* remoteHookProc;
} sHook;


bool hookAlloc( HANDLE hProcess, wchar_t* libName, char* procName, sHook& rHook );
bool hookAttach( HANDLE hProcess, wchar_t* libName, char* procName, void* localHookProc, sHook& rHook );
bool hookFree( HANDLE hProcess, wchar_t* libName, char* procName, sHook& rHook );
