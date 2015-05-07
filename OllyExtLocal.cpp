#include "stdafx.h"
#include "OllyExtLocal.h"


HMODULE localLoadLibrary( const wchar_t* libName )
{
	HMODULE hModule = LoadLibrary( libName );
	PLUGIN_CHECK_ERROR( hModule, return NULL;, UNABLE_TO_LOAD_MODULE, libName );
	return hModule;
}


HMODULE localGetModuleHandle( const wchar_t* libName )
{
	HMODULE hModule = GetModuleHandle( libName );
	PLUGIN_CHECK_ERROR( hModule, return NULL;, UNABLE_TO_FIND_MODULE, libName );
	return hModule;
}


FARPROC localGetProcAddress( const wchar_t* libName, const char* procName )
{
	HMODULE hModule = localLoadLibrary( libName );
	if( !hModule ) return NULL;

	FARPROC procAddress = GetProcAddress( hModule, procName );
	if( !procAddress )
	{
		wchar_t procNameW[TEXTLEN];
		Asciitounicode( procName, strlen( procName ), procNameW, TEXTLEN );
		PLUGIN_CHECK_ERROR( FALSE, return NULL;, UNABLE_TO_FIND_FUNCTION, libName, procNameW );
	}

	return procAddress;
}


void localGetProcIdList( const wchar_t* procName, std::vector<DWORD>& rList )
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE ) return;

	pe32.dwSize = sizeof( PROCESSENTRY32 );

	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		CloseHandle( hProcessSnap );
		return;
	}

	do
	{
		if( wcsstr( pe32.szExeFile, procName ) )
			rList.push_back( pe32.th32ProcessID );
	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );
}
