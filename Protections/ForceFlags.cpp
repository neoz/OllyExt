#include "stdafx.h"
#include "..\OllyExtRemote.h"
#include "ForceFlags.h"


#define PROCESS_HEAP_OFFSET ( 0x18 )
#define FORCE_FLAGS_OFFSET_XP ( 0x10 )
#define FORCE_FLAGS_OFFSET_WIN7 ( 0x44 )


static bool g_applied = false;
static BYTE g_patch[] = { 0x00, 0x00, 0x00, 0x00 };
static BYTE g_orig[sizeof( g_patch )] = { 0 };


void forceFlagsReset( void )
{
	g_applied = false;
}


void forceFlagsApply( bool protect )
{
	if( protect )
	{
		if( g_applied ) return;

		DWORD peb = remoteGetPEB( process );
		if( !peb ) return;

		DWORD processHeapOffset = peb + PROCESS_HEAP_OFFSET;
		DWORD processHeap = 0;
		BOOL readSuccess = ReadProcessMemory( process, (void*)processHeapOffset, &processHeap, sizeof( processHeap ), NULL );
		PLUGIN_CHECK_ERROR( readSuccess, return;, UNABLE_TO_READ_MEMORY_AT, processHeapOffset );

		DWORD forceFlagsOffset = 0;
		eOSType osType = osGetType();
		switch( osType )
		{
			case OS_XP:
				forceFlagsOffset = FORCE_FLAGS_OFFSET_XP;
			break;

			case OS_WIN7:
				forceFlagsOffset = FORCE_FLAGS_OFFSET_WIN7;
			break;

			default:
				forceFlagsOffset = 0;
			break;
		}

		if( !forceFlagsOffset ) return;

		if( !remotePatchApply( process, (void*)( processHeap + forceFlagsOffset ), g_orig, g_patch, sizeof( g_patch ) ) )
			return;

		g_applied = true;
	}
	else
	{
		if( !g_applied ) return;

		DWORD peb = remoteGetPEB( process );
		if( !peb ) return;

		DWORD processHeapOffset = peb + PROCESS_HEAP_OFFSET;
		DWORD processHeap = 0;
		BOOL readSuccess = ReadProcessMemory( process, (void*)processHeapOffset, &processHeap, sizeof( processHeap ), NULL );
		PLUGIN_CHECK_ERROR( readSuccess, return;, UNABLE_TO_READ_MEMORY_AT, processHeapOffset );

		DWORD forceFlagsOffset = 0;
		eOSType osType = osGetType();
		switch( osType )
		{
			case OS_XP:
				forceFlagsOffset = FORCE_FLAGS_OFFSET_XP;
			break;

			case OS_WIN7:
				forceFlagsOffset = FORCE_FLAGS_OFFSET_WIN7;
			break;

			default:
				forceFlagsOffset = 0;
			break;
		}

		if( !forceFlagsOffset ) return;

		if( !remotePatchApply( process, (void*)( processHeap + forceFlagsOffset ), NULL, g_orig, sizeof( g_orig ) ) )
			return;

		g_applied = false;
	}
}
