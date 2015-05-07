#include "stdafx.h"
#include "..\OllyExtRemote.h"
#include "IsDebuggerPresent.h"


#define BEING_DEBUGGED_OFFSET ( 0x02 )


static bool g_applied = false;
static BYTE g_patch[] = { 0 };
static BYTE g_orig[sizeof( g_patch )] = { 0 };


void isDebuggerPresentReset( void )
{
	g_applied = false;
}


void isDebuggerPresentApply( bool protect )
{
	if( protect )
	{
		if( g_applied ) return;

		DWORD peb = remoteGetPEB( process );
		if( !peb ) return;

		if( !remotePatchApply( process, (void*)( peb + BEING_DEBUGGED_OFFSET ), g_orig, g_patch, sizeof( g_patch ) ) )
			return;

		g_applied = true;
	}
	else
	{
		if( !g_applied ) return;

		DWORD peb = remoteGetPEB( process );
		if( !peb ) return;

		if( !remotePatchApply( process, (void*)( peb + BEING_DEBUGGED_OFFSET ), NULL, g_orig, sizeof( g_orig ) ) )
			return;

		g_applied = false;
	}
}
