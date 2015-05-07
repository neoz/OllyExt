#pragma once


HMODULE localLoadLibrary( const wchar_t* libName );
HMODULE localGetModuleHandle( const wchar_t* libName );
FARPROC localGetProcAddress( const wchar_t* libName, const char* procName );
void localGetProcIdList( const wchar_t* procName, std::vector<DWORD>& rList );
