#include "stdafx.h"
#include "..\OllyExtRemote.h"
#include "NtGlobalFlag.h"


#define REG_KEY_NAME_LEN ( 4096 )
#define REG_KEY_NAME ( L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options" )
#define NT_GLOBAL_FLAG_OFFSET ( 0x68 )


static bool g_applied = false;
static BYTE g_patch[] = { 0 };
static BYTE g_orig[sizeof( g_patch )] = { 0 };


void ntGlobalFlagPreHook( bool protect )
{
	if( !protect ) return;

	HKEY hKey = NULL;

	wchar_t* regKeyName = new wchar_t[REG_KEY_NAME_LEN];
	PLUGIN_CHECK_ERROR( regKeyName, return;, UNABLE_TO_ALLOCATE_LOCAL_MEMORY );

	const wchar_t* pFileName = PathFindFileName( executable );
	PLUGIN_CHECK_ERROR( wcslen( pFileName ) > 0, return;, NO_DEBUGGE_NAME );
	swprintf_s( regKeyName, REG_KEY_NAME_LEN, L"%s\\%s", REG_KEY_NAME, pFileName );

	SECURITY_ATTRIBUTES sAttribs = { sizeof( SECURITY_ATTRIBUTES ) };
	DWORD result = RegCreateKeyEx( HKEY_LOCAL_MACHINE, regKeyName, 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, &sAttribs, &hKey, NULL );
	PLUGIN_CHECK_ERROR( result == ERROR_SUCCESS,
		delete [] regKeyName; \
		return;,
		UNABLE_TO_OPEN_OR_CREATE_REGISTRY_KEY, regKeyName, result );

	DWORD globalFlag = 0;
//	Only RegSetValueEx is supported on XP
//	result = RegSetKeyValue( hKey, NULL, L"GlobalFlag", REG_DWORD, (LPCVOID)&globalFlag, sizeof( globalFlag ) );
	result = RegSetValueEx( hKey, L"GlobalFlag", 0, REG_DWORD, (const BYTE*)&globalFlag, sizeof( globalFlag ) );
	PLUGIN_CHECK_ERROR( result == ERROR_SUCCESS,
		delete [] regKeyName; \
		return;,
		UNABLE_TO_SET_REGISTRY_KEY, regKeyName, L"GlobalFlag", result );

	result = RegCloseKey( hKey );
	PLUGIN_CHECK_ERROR( result == ERROR_SUCCESS,
		delete [] regKeyName; \
		return;,
		UNABLE_TO_CLOSE_REGISTRY_KEY, REG_KEY_NAME, result );

	delete [] regKeyName;
}


void ntGlobalFlagPostHook( bool protect )
{
	if( !protect ) return;

	HKEY hKey = NULL;

	const wchar_t* pFileName = PathFindFileName( executable );
	PLUGIN_CHECK_ERROR( wcslen( pFileName ) > 0, return;, NO_DEBUGGE_NAME );

	DWORD result = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REG_KEY_NAME, NULL, KEY_ALL_ACCESS, &hKey );
	if( result != ERROR_SUCCESS )
	{
		return;
	}

//	Only SHDeleteKey is supported on XP
//	RegDeleteTree( hKey, pFileName );
	SHDeleteKey( hKey, pFileName );
	RegCloseKey( hKey );
}


void ntGlobalFlagReset( void )
{
	g_applied = false;
}


void ntGlobalFlagApply( bool protect )
{
	if( protect )
	{
		if( g_applied ) return;

		DWORD peb = remoteGetPEB( process );
		if( !peb ) return;

		if( !remotePatchApply( process, (void*)( peb + NT_GLOBAL_FLAG_OFFSET ), g_orig, g_patch, sizeof( g_patch ) ) )
			return;

		g_applied = true;
	}
	else
	{
		if( !g_applied ) return;

		DWORD peb = remoteGetPEB( process );
		if( !peb ) return;

		if( !remotePatchApply( process, (void*)( peb + NT_GLOBAL_FLAG_OFFSET ), NULL, g_orig, sizeof( g_orig ) ) )
			return;

		g_applied = false;
	}
}
