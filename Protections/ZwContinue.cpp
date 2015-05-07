#include "stdafx.h"
#include "..\OllyExtHook.h"
#include "..\OllyExtAssembler.h"
#include "ZwContinue.h"
#include "KiUserExceptionDispatcher.h"


#define ASSEMBLER_ERROR_HANDLER() \
	delete [] localHookProc; \
	hookFree( process, LIBNAME, PROCNAME, g_hook ); \
	return;

#define LIBNAME ( L"ntdll.dll" )
#define PROCNAME ( "ZwContinue" )
#define HOOK_SIZE ( 0x100 )


static bool g_applied = false;
static sHook g_hook = { 0 };


void zwContinueReset( void )
{
	g_applied = false;
}


void zwContinueApply( bool protect )
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

		// Check wheather we saved something
		ASSEMBLER_SET_LABEL( "registersChangedPatch", localEip );
		ASSEMBLE_AND_STEP( L"mov eax, 0x12345678", localEip );
		ASSEMBLE_AND_STEP( L"cmp byte ptr[eax], 0x00", localEip );
		ASSEMBLER_SET_LABEL( "notSavedPatch", localEip );
		wsprintf( cmd, L"jz 0x%08X", codeBase + localEip );
		ASSEMBLE_AND_STEP( cmd, localEip );
		ASSEMBLE_AND_STEP( L"mov byte ptr[eax], 0x00", localEip );

		// Restore DRX registers
		ASSEMBLE_AND_STEP( L"push esi", localEip );
		ASSEMBLE_AND_STEP( L"push edi", localEip );

		ASSEMBLER_SET_LABEL( "registersPatch", localEip );
		ASSEMBLE_AND_STEP( L"mov esi, 0x12345678", localEip );
		ASSEMBLE_AND_STEP( L"mov edi, [esp + 0x0C]", localEip );
		ASSEMBLE_AND_STEP( L"add edi, 0x04", localEip );
		ASSEMBLE_AND_STEP( L"mov ecx, 0x06", localEip );
		ASSEMBLE_AND_STEP( L"rep movsd", localEip );

		ASSEMBLE_AND_STEP( L"pop edi", localEip );
		ASSEMBLE_AND_STEP( L"pop esi", localEip );

		ASSEMBLER_SET_LABEL( "notSavedLabel", localEip );

		// Go to original handler with faked DRX registers
		wsprintf( cmd, L"jmp 0x%08X", g_hook.trampolineProc );
		ASSEMBLE_AND_STEP( cmd, localEip );

		// Resolve addresses which only known at the end
		patchEip = ASSEMBLER_GET_LABEL( "registersChangedPatch" );
		wsprintf( cmd, L"mov eax, 0x%08X", g_drxRegistersChangedAddress );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "notSavedPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "notSavedLabel" );
		wsprintf( cmd, L"jz 0x%08X", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "registersPatch" );
		wsprintf( cmd, L"mov esi, 0x%08X", g_drxRegistersAddress );
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
