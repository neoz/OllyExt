#include "stdafx.h"
#include "..\OllyExtRemote.h"
#include "HeapFlags.h"


#define PROCESS_HEAP_OFFSET ( 0x18 )
#define FLAGS_OFFSET_XP ( 0x0C )
#define FLAGS_OFFSET_WIN7 ( 0x40 )


static bool g_applied = false;
static BYTE g_patch[] = { 0x02, 0x00, 0x00, 0x00 };
static BYTE g_orig[sizeof( g_patch )] = { 0 };


void heapFlagsReset( void )
{
	g_applied = false;
}


void heapFlagsApply( bool protect )
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

		DWORD flagsOffset = 0;
		eOSType osType = osGetType();
		switch( osType )
		{
			case OS_XP:
				flagsOffset = FLAGS_OFFSET_XP;
			break;

			case OS_WIN7:
				flagsOffset = FLAGS_OFFSET_WIN7;
			break;

			default:
				flagsOffset = 0;
			break;
		}

		if( !flagsOffset ) return;

		if( !remotePatchApply( process, (void*)( processHeap + flagsOffset ), g_orig, g_patch, sizeof( g_patch ) ) )
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

		DWORD flagsOffset = 0;
		eOSType osType = osGetType();
		switch( osType )
		{
			case OS_XP:
				flagsOffset = FLAGS_OFFSET_XP;
			break;

			case OS_WIN7:
				flagsOffset = FLAGS_OFFSET_WIN7;
			break;

			default:
				flagsOffset = 0;
			break;
		}

		if( !flagsOffset ) return;

		if( !remotePatchApply( process, (void*)( processHeap + flagsOffset ), NULL, g_orig, sizeof( g_orig ) ) )
			return;

		g_applied = false;
	}
}
