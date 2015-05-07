#include "stdafx.h"
#include "OllyExtBugfixes.h"

#include "Caption.h"
#include "KillAntiAttach.h"


sBugfixesOptions g_bugfixOptions = { 0 };


#define CAPTION L"Caption"
#define KILLANTIATTACH L"KillAntiAttach"

#define GETFROMINI( feature, option ) \
{ \
	DWORD data = 0; \
	Getfromini( NULL, PLUGIN_NAME, feature, L"%d", &data );\
	option = ( data != 0 ); \
}


void bugfixesReadOptions( void )
{
	GETFROMINI( CAPTION, g_bugfixOptions.caption );
	GETFROMINI( KILLANTIATTACH, g_bugfixOptions.killAntiAttach );
}


void bugfixesWriteOptions( void )
{
	Writetoini( NULL, PLUGIN_NAME, CAPTION, L"%d", g_bugfixOptions.caption );
	Writetoini( NULL, PLUGIN_NAME, KILLANTIATTACH, L"%d", g_bugfixOptions.killAntiAttach );
}


bool bugfixesApply( void )
{
	captionApply( g_bugfixOptions.caption );
	killAntiAttachApply( g_bugfixOptions.killAntiAttach );

	return true;
}
