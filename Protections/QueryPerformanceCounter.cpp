#include "stdafx.h"
#include "..\OllyExtHook.h"
#include "..\OllyExtAssembler.h"
#include "..\OllyExtRemote.h"
#include "QueryPerformanceCounter.h"


#define ASSEMBLER_ERROR_HANDLER() \
	delete [] localHookProc; \
	hookFree( process, LIBNAME, PROCNAME, g_hook ); \
	return;

#define LIBNAME ( L"kernel32.dll" )
#define PROCNAME ( "QueryPerformanceCounter" )
#define HOOK_SIZE ( 0x100 )


static bool g_applied = false;
static sHook g_hook = { 0 };


void queryPerformanceCounterReset( void )
{
	g_applied = false;
}


void queryPerformanceCounterApply( bool protect )
{
	if( protect )
	{
		if( g_applied ) return;

		wchar_t procNameW[TEXTLEN];
		Asciitounicode( PROCNAME, strlen( PROCNAME ), procNameW, TEXTLEN );

		g_hook.remoteTargetProc = NULL;
		g_hook.trampolineProcSize = 0;
		g_hook.trampolineProc = NULL;
		g_hook.hookSize = HOOK_SIZE;
		g_hook.remoteHookProc = NULL;

		bool allocResult = hookAlloc( process, LIBNAME, PROCNAME, g_hook );
		PLUGIN_CHECK_ERROR( allocResult, return;, UNABLE_TO_ALLOC_HOOK, LIBNAME, procNameW );

		BYTE* localHookProc = new BYTE[HOOK_SIZE];
		PLUGIN_CHECK_ERROR( localHookProc,
			hookFree( process, LIBNAME, PROCNAME, g_hook ); \
			return;,
			UNABLE_TO_ALLOCATE_LOCAL_MEMORY );

		wchar_t cmd[64] = { 0 };
		DWORD codeBase = (DWORD)g_hook.remoteHookProc;
		DWORD localEip = 0;
		DWORD patchEip = 0;
		DWORD labelEip = 0;

		// Assembler init
		ASSEMBLER_FLUSH();
		ASSEMBLER_SET_CODEBASE( codeBase );
		ASSEMBLER_SET_TARGET_BUFFER( localHookProc );

		// Return and clean the stack
		ASSEMBLE_AND_STEP( L"mov edx, dword ptr[esp + 0x04]", localEip );
		ASSEMBLER_SET_LABEL( "movCounterLowPatch", localEip );
		ASSEMBLE_AND_STEP( L"mov eax, dword ptr[0]", localEip );
		ASSEMBLE_AND_STEP( L"mov dword ptr[edx], eax", localEip );
		ASSEMBLE_AND_STEP( L"add edx, 0x04", localEip );
		ASSEMBLER_SET_LABEL( "movCounterHighPatch", localEip );
		ASSEMBLE_AND_STEP( L"mov eax, dword ptr[0]", localEip );
		ASSEMBLE_AND_STEP( L"mov dword ptr[edx], eax", localEip );
		ASSEMBLER_SET_LABEL( "readIncPatch", localEip );
		ASSEMBLE_AND_STEP( L"mov edx, dword ptr[0]", localEip );
		ASSEMBLER_SET_LABEL( "incCounterLowPatch", localEip );
		ASSEMBLE_AND_STEP( L"add dword ptr[0], edx", localEip );
		ASSEMBLER_SET_LABEL( "incCounterHighPatch", localEip );
		ASSEMBLE_AND_STEP( L"adc dword ptr[0], 0", localEip );
		ASSEMBLE_AND_STEP( L"mov eax, 0x01", localEip );
		ASSEMBLE_AND_STEP( L"ret 0x04", localEip );
		LARGE_INTEGER li = { 0 };
		QueryPerformanceCounter( &li );
		ASSEMBLER_SET_LABEL( "counterLowAddress", localEip );
		ASSEMBLER_ADD_DWROD_AND_STEP( li.LowPart, localEip );
		ASSEMBLER_SET_LABEL( "counterHighAddress", localEip );
		ASSEMBLER_ADD_DWROD_AND_STEP( li.HighPart, localEip );
		ASSEMBLER_SET_LABEL( "incAddress", localEip );
		ASSEMBLER_ADD_DWROD_AND_STEP( 1, localEip );

		// Resolve addresses which only known at the end
		patchEip = ASSEMBLER_GET_LABEL( "movCounterLowPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "counterLowAddress" );
		wsprintf( cmd, L"mov eax, dword ptr[0x%08X]", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "movCounterHighPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "counterHighAddress" );
		wsprintf( cmd, L"mov eax, dword ptr[0x%08X]", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "readIncPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "incAddress" );
		wsprintf( cmd, L"mov edx, dword ptr[0x%08X]", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "incCounterLowPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "counterLowAddress" );
		wsprintf( cmd, L"add dword ptr[0x%08X], edx", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "incCounterHighPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "counterHighAddress" );
		wsprintf( cmd, L"adc dword ptr[0x%08X], 0", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		ASSEMBLER_FLUSH();

		bool attachResult = hookAttach( process, LIBNAME, PROCNAME, localHookProc, g_hook );
		PLUGIN_CHECK_ERROR( attachResult,
			delete [] localHookProc; \
			hookFree( process, LIBNAME, PROCNAME, g_hook ); \
			return;,
			UNABLE_TO_ATTACH_HOOK, LIBNAME, procNameW );

		delete [] localHookProc;

		g_applied = true;
	}
	else
	{
		if( !g_applied ) return;

		wchar_t procNameW[TEXTLEN];
		Asciitounicode( PROCNAME, strlen( PROCNAME ), procNameW, TEXTLEN );

		bool freeResult = hookFree( process, LIBNAME, PROCNAME, g_hook );
		PLUGIN_CHECK_ERROR( freeResult, return;, UNABLE_TO_FREE_HOOK, LIBNAME, procNameW );

		g_applied = false;
	}
}
