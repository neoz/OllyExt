#include "stdafx.h"
#include "..\OllyExtHook.h"
#include "..\OllyExtAssembler.h"
#include "ZwGetContextThread.h"
#include "NtSetContextThread.h"


#define ASSEMBLER_ERROR_HANDLER() \
	delete [] localHookProc; \
	hookFree( process, LIBNAME, PROCNAME, g_hook ); \
	return;

#define LIBNAME ( L"ntdll.dll" )
#define PROCNAME ( "NtSetContextThread" )
#define HOOK_SIZE ( 0x400 )


static bool g_applied = false;
static sHook g_hook = { 0 };


void ntSetContextThreadReset( void )
{
	g_applied = false;
}


void ntSetContextThreadApply( bool protect )
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

		// Get current context
		void* zwGetContextThread = zwGetContextThreadOrig();
		if( !zwGetContextThread )
		{
			ASSEMBLER_ERROR_HANDLER();
		}
		ASSEMBLER_SET_LABEL( "contextPatch", localEip );
		ASSEMBLE_AND_STEP( L"push 0x12345678", localEip );
		ASSEMBLE_AND_STEP( L"push dword ptr[esp + 0x08]", localEip );
		wsprintf( cmd, L"call 0x%08X", zwGetContextThread );
		ASSEMBLE_AND_STEP( cmd, localEip );

		// Set DRX registers with the actual values
		ASSEMBLE_AND_STEP( L"push esi", localEip );
		ASSEMBLE_AND_STEP( L"push edi", localEip );
		ASSEMBLER_SET_LABEL( "copyPatch", localEip );
		ASSEMBLE_AND_STEP( L"mov esi, 0x12345678", localEip );
		ASSEMBLE_AND_STEP( L"mov edi, [esp + 0x10]", localEip );
		ASSEMBLE_AND_STEP( L"add edi, 0x04", localEip );
		ASSEMBLE_AND_STEP( L"mov ecx, 0x06", localEip );
		ASSEMBLE_AND_STEP( L"rep movsd", localEip );
		ASSEMBLE_AND_STEP( L"pop edi", localEip );
		ASSEMBLE_AND_STEP( L"pop esi", localEip );

		// Call original function
		ASSEMBLE_AND_STEP( L"mov eax, esp", localEip );
		ASSEMBLE_AND_STEP( L"mov ecx, 0x02", localEip );
		ASSEMBLER_SET_LABEL( "loopLabel", localEip );
		ASSEMBLE_AND_STEP( L"mov edx, [ecx*4+eax]", localEip );
		ASSEMBLE_AND_STEP( L"push edx", localEip );
		ASSEMBLE_AND_STEP( L"dec ecx", localEip );
		ASSEMBLE_AND_STEP( L"cmp ecx, 0x00", localEip );
		labelEip = ASSEMBLER_GET_LABEL( "loopLabel" );
		wsprintf( cmd, L"jnz 0x%08X", codeBase + labelEip );
		ASSEMBLE_AND_STEP( cmd, localEip );
		wsprintf( cmd, L"call 0x%08X", g_hook.trampolineProc );
		ASSEMBLE_AND_STEP( cmd, localEip );

		// Return and clean the stack
		ASSEMBLE_AND_STEP( L"ret 0x08", localEip );

		// Add space for context
		localEip = ( localEip + ( 4 - 1 ) ) & -4;
		CONTEXT context = { CONTEXT_DEBUG_REGISTERS };
		ASSEMBLER_SET_LABEL( "contextLabel", localEip );
		ASSEMBLER_ADD_ARRAY_AND_STEP( &context, sizeof( CONTEXT ), localEip );

		// Resolve addresses which only known at the end
		labelEip = ASSEMBLER_GET_LABEL( "contextLabel" );

		patchEip = ASSEMBLER_GET_LABEL( "contextPatch" );
		wsprintf( cmd, L"push 0x%08X", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "copyPatch" );
		wsprintf( cmd, L"mov esi, 0x%08X", codeBase + labelEip + 0x04 );
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
