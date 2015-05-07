#include "stdafx.h"
#include "OllyExtVersion.h"


#define COPYRIGHT_BUF_LEN ( 2048 )


int g_ollydbgVersion = 0;
wchar_t* g_versionStr = NULL;
wchar_t* g_copyrightStr = NULL;


void versionInit( void )
{
	g_versionStr = new wchar_t[TEXTLEN];
	wsprintf( g_versionStr, L"%ws v%ws compiled %ws %ws", PLUGIN_NAME, OLLYEXT_VERSION, _T( __DATE__ ), _T( __TIME__ ) );
	g_copyrightStr = new wchar_t[COPYRIGHT_BUF_LEN];
	wsprintf( g_copyrightStr, L"Copyright (C) 2013 Ferrit <ferrit.rce@gmail.com>" );
	wcscat_s( g_copyrightStr, COPYRIGHT_BUF_LEN, L"\n\nA couple of code parts has taken from the famous OllyAdvanced source. Thanks to MaRKuS TH-DJM!" );
	wcscat_s( g_copyrightStr, COPYRIGHT_BUF_LEN, L"\n\nSpecial thanks and greetings to Oleh Yuschuk, Cenzo, HellSp@wn, Teddy Rogers, BeaEngine Team!" );
}


void versionDestroy( void )
{
	if( g_versionStr )
	{
		delete [] g_versionStr;
		g_versionStr = NULL;
	}
	if( g_copyrightStr )
	{
		delete [] g_copyrightStr;
		g_copyrightStr = NULL;
	}
}
