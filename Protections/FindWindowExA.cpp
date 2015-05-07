#include "stdafx.h"
#include "..\OllyExtHook.h"
#include "..\OllyExtAssembler.h"
#include "..\OllyExtRemote.h"
#include "FindWindowExA.h"


#define ASSEMBLER_ERROR_HANDLER() \
	delete [] localHookProc; \
	hookFree( process, LIBNAME, PROCNAME, g_hook ); \
	return;

#define LIBNAME ( L"user32.dll" )
#define PROCNAME ( "FindWindowExA" )
#define HOOK_SIZE ( 0x200 )
#define CLASS_NAME ( "OLLYDBG" )

static bool g_applied = false;
static sHook g_hook = { 0 };


void findWindowExAReset( void )
{
	g_applied = false;
}


void findWindowExAApply( bool protect )
{
	if( protect )
	{
		if( g_applied ) return;

		HMODULE hModule = remoteGetModuleHandle( process, LIBNAME );
		if( !hModule ) return;

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

		// Push used registers
		ASSEMBLE_AND_STEP( L"push esi", localEip );
		ASSEMBLE_AND_STEP( L"push edi", localEip );

		// If lpClassName is OLLYDBG
		ASSEMBLE_AND_STEP( L"mov esi, dword ptr[esp + 0x14]", localEip );
		ASSEMBLE_AND_STEP( L"cmp esi, 0x00", localEip );
		ASSEMBLER_SET_LABEL( "nullClassNamePatch", localEip );
		wsprintf( cmd, L"jz 0x%08X", codeBase + localEip );
		ASSEMBLE_AND_STEP( cmd, localEip );
		ASSEMBLER_SET_LABEL( "classNamePatch", localEip );
		wsprintf( cmd, L"mov edi, 0x%08X", codeBase + localEip );
		ASSEMBLE_AND_STEP( cmd, localEip );
		ASSEMBLER_SET_LABEL( "loopLabel", localEip );
		ASSEMBLE_AND_STEP( L"mov al, byte ptr[esi]", localEip );
		ASSEMBLE_AND_STEP( L"mov dl, byte ptr[edi]", localEip );
		ASSEMBLE_AND_STEP( L"inc esi", localEip );
		ASSEMBLE_AND_STEP( L"inc edi", localEip );
		ASSEMBLE_AND_STEP( L"cmp al, dl", localEip );
		ASSEMBLER_SET_LABEL( "notSameClassPatch", localEip );
		wsprintf( cmd, L"jnz 0x%08X", codeBase + localEip );
		ASSEMBLE_AND_STEP( cmd, localEip );
		ASSEMBLE_AND_STEP( L"cmp al, 0x00", localEip );
		ASSEMBLER_SET_LABEL( "sameClassPatch", localEip );
		wsprintf( cmd, L"jz 0x%08X", codeBase + localEip );
		ASSEMBLE_AND_STEP( cmd, localEip );
		labelEip = ASSEMBLER_GET_LABEL( "loopLabel" );
		wsprintf( cmd, L"jmp 0x%08X", codeBase + labelEip );
		ASSEMBLE_AND_STEP( cmd, localEip );

		// Pop used registers
		ASSEMBLER_SET_LABEL( "nullClassNameLabel", localEip );
		ASSEMBLER_SET_LABEL( "notSameClassLabel", localEip );
		ASSEMBLE_AND_STEP( L"pop edi", localEip );
		ASSEMBLE_AND_STEP( L"pop esi", localEip );

		// Call original function
		ASSEMBLE_AND_STEP( L"mov eax, esp", localEip );
		ASSEMBLE_AND_STEP( L"mov ecx, 0x04", localEip );
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
		ASSEMBLE_AND_STEP( L"ret 0x10", localEip );

		// Return and clean the stack
		ASSEMBLER_SET_LABEL( "sameClassLabel", localEip );
		ASSEMBLE_AND_STEP( L"pop edi", localEip );
		ASSEMBLE_AND_STEP( L"pop esi", localEip );
		ASSEMBLE_AND_STEP( L"xor eax, eax", localEip );
		ASSEMBLE_AND_STEP( L"ret 0x10", localEip );

		// Add strings
		ASSEMBLER_SET_LABEL( "classNameLabel", localEip );
		ASSEMBLER_ADD_STRING_AND_STEP( CLASS_NAME, localEip );

		// Resolve addresses which only known at the end
		patchEip = ASSEMBLER_GET_LABEL( "nullClassNamePatch" );
		labelEip = ASSEMBLER_GET_LABEL( "nullClassNameLabel" );
		wsprintf( cmd, L"jz 0x%08X", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "classNamePatch" );
		labelEip = ASSEMBLER_GET_LABEL( "classNameLabel" );
		wsprintf( cmd, L"mov edi, 0x%08X", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "notSameClassPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "notSameClassLabel" );
		wsprintf( cmd, L"jnz 0x%08X", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "sameClassPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "sameClassLabel" );
		wsprintf( cmd, L"jz 0x%08X", codeBase + labelEip );
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
