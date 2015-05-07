#pragma once


DWORD remoteGetPEB( HANDLE hProcess );
void* remoteAlloc( HANDLE hProcess );
void remoteFree( HANDLE hProcess, void* remoteAddress );
bool remoteRead( HANDLE hProcess, void* remoteAddress, DWORD remoteSize, void* localAddress );
bool remoteWrite( HANDLE hProcess, const void* localAddress, DWORD localSize, void* remoteAddress );
bool remoteExec( HANDLE hProcess, void* remoteParamAddress, void* remoteCodeAddress );
HMODULE remoteGetModuleHandle( HANDLE hProcess, const wchar_t* libName );
FARPROC remoteGetProcAddress( HANDLE hProcess, const wchar_t* libName, const char* procName );
bool remotePatchApply( HANDLE hProcess, void* remoteAddress, BYTE* origCode, BYTE* patchCode, DWORD size );
bool remotePatchApply( HANDLE hProcess, const wchar_t* libName, const char* procName, BYTE* origCode, BYTE* patchCode, DWORD size );
