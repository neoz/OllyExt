#include "stdafx.h"
#include "OllyExtVersion.h"
#include "OllyExtAbout.h"


#define ABOUT_BUF_LEN ( 2048 )


wchar_t* g_aboutStr = NULL;


void aboutInit( void )
{
	g_aboutStr = new wchar_t[ABOUT_BUF_LEN];
	wsprintf( g_aboutStr, L"%ws\n%ws", g_versionStr, g_copyrightStr );
}


void aboutShow( void )
{
	MSGBOXPARAMS mbp;
	mbp.cbSize = sizeof( MSGBOXPARAMS );
	mbp.hwndOwner = hwollymain;
	mbp.hInstance = g_instance;
	mbp.lpszText = g_aboutStr;
	mbp.lpszCaption = PLUGIN_NAME;
	mbp.dwStyle = MB_OK | MB_TOPMOST | MB_APPLMODAL | MB_USERICON;
	mbp.dwLanguageId = MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT );
	mbp.lpfnMsgBoxCallback = NULL;
	mbp.dwContextHelpId = 0;
	mbp.lpszIcon = MAKEINTRESOURCE( IDI_ICON );
	MessageBoxIndirect( &mbp );
}


void aboutDestroy( void )
{
	if( g_aboutStr )
	{
		delete [] g_aboutStr;
		g_aboutStr = NULL;
	}
}
