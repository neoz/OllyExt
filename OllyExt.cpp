#include "stdafx.h"
#include "OllyExtOS.h"
#include "OllyExtVersion.h"
#include "OllyExtAbout.h"
#include "OllyExtIcon.h"
#include "OllyExtMenu.h"
#include "OllyExtDisasmMenu.h"
#include "OllyExtDumpMenu.h"
#include "OllyExtIntegrity.h"
#include "OllyExtProtect.h"
#include "OllyExtBugfixes.h"
#include "OllyExtGeneral.h"
#include "OllyExt.h"


HINSTANCE g_instance = NULL;
static bool g_protectionApplied = false;


BOOL WINAPI DllMain( HINSTANCE new_instance, DWORD fdwReason, LPVOID lpvReserved )
{
	g_instance = new_instance;
	return TRUE;
}


extc int __cdecl ODBG2_Pluginquery( int ollydbgversion, ulong* features, wchar_t pluginname[SHORTNAME], wchar_t pluginversion[SHORTNAME] )
{
	if( ollydbgversion < 201 )
	{
		PLUGIN_CHECK_ERROR( false, return 0;, INVALID_OLLY_VERSION, 201, ollydbgversion );
	}
	g_ollydbgVersion = ollydbgversion;

	osInit();

	versionInit();

	wcscpy_s( pluginname, SHORTNAME, PLUGIN_NAME );
	wcscpy_s( pluginversion, SHORTNAME, OLLYEXT_VERSION );

	Addtolist( 0, BLACK, g_versionStr );

    wchar_t v[TEXTLEN];
    mbstowcs( v, BeaEngineVersion(), TEXTLEN );
	wchar_t beaVer[TEXTLEN];
	wsprintf( beaVer, L"%ws: BeaEngine version: %ws", PLUGIN_NAME, v );
	Addtolist( 0, BLACK, beaVer );

	wchar_t r[TEXTLEN];
    mbstowcs( r, BeaEngineRevision(), TEXTLEN );
	wchar_t beaRev[TEXTLEN];
	wsprintf( beaRev, L"%ws: BeaEngine revision: %ws", PLUGIN_NAME, r );
	Addtolist( 0, BLACK, beaRev );

	return PLUGIN_VERSION;
}


extc int __cdecl ODBG2_Plugininit( void )
{
	aboutInit();

	iconInit();
	SendMessage( hwollymain, WM_SETICON, ICON_SMALL, (LPARAM)g_hIconSmall );
	SendMessage( hwollymain, WM_SETICON, ICON_BIG, (LPARAM)g_hIconBig );

	integrityInit();
	integritySnapshot( L"ntdll.dll" );

	protectReadOptions();

	bugfixesReadOptions();
	bugfixesApply();

	generalReadOptions();

	return 0;
}


extc void ODBG2_Pluginnotify( int code, void* data, ulong parm1, ulong parm2 )
{
	switch( code )
	{
		case PN_STATUS:
			switch( parm1 )
			{
				case STAT_LOADING:
					protectPreHook();
				break;

				case STAT_IDLE:
					protectPostHook();
				break;
			}
		break;
	}
}


extc void __cdecl ODBG2_Pluginreset( void )
{
	g_protectionApplied = false;
	protectReset();
}


extc int __cdecl ODBG2_Pluginexception( t_run* prun, const t_disasm* da, t_thread* pthr, t_reg* preg, wchar_t* message )
{
	if( !g_protectionApplied &&
		!skipsystembp )
	{
		g_protectionApplied = true;
		protectApply();
	}

	return PE_IGNORED;
}


extc t_menu* __cdecl ODBG2_Pluginmenu( wchar_t* type )
{
	if( !wcscmp( type, PWM_MAIN ) )
		return g_mainMenu;
	else if( !wcscmp( type, PWM_DISASM ) )
		return g_disasmMenu;
	else if( !wcscmp( type, PWM_DUMP ) )
		return g_dumpMenu;
	return NULL;
};


extc void __cdecl ODBG2_Plugindestroy( void )
{
	integrityDestroy();
	iconDestroy();
	aboutDestroy();
	versionDestroy();
}
