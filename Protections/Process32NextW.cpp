#include "stdafx.h"
#include "..\OllyExtLocal.h"
#include "..\OllyExtHook.h"
#include "..\OllyExtAssembler.h"
#include "Process32NextW.h"


#define ASSEMBLER_ERROR_HANDLER() \
	delete [] localHookProc; \
	hookFree( process, LIBNAME, PROCNAME, g_hook ); \
	return;

#define LIBNAME ( L"kernel32.dll" )
#define PROCNAME ( "Process32NextW" )
#define HOOK_SIZE ( 0x100 )


static bool g_applied = false;
static sHook g_hook = { 0 };


void process32NextWReset( void )
{
	g_applied = false;
}


void process32NextWApply( bool protect )
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

		// If lppe->th32ModuleID is protected
		ASSEMBLE_AND_STEP( L"mov edx, dword ptr[esp + 0x08]", localEip );
		DWORD pid = GetCurrentProcessId();
		wsprintf( cmd, L"cmp dword ptr[edx + 0x08], 0x%08X", pid );
		ASSEMBLE_AND_STEP( cmd, localEip );
		ASSEMBLER_SET_LABEL( "processNoSkipPatch", localEip );
		wsprintf( cmd, L"jnz 0x%08X", codeBase + localEip );
		ASSEMBLE_AND_STEP( cmd, localEip );
		ASSEMBLER_SET_LABEL( "processSkipPatch", localEip );
		wsprintf( cmd, L"call 0x12345678" );
		ASSEMBLE_AND_STEP( cmd, localEip );

		// If lppe->th32ParentProcessID is protected
		ASSEMBLER_SET_LABEL( "processNoSkipLabel", localEip );
		ASSEMBLE_AND_STEP( L"mov edx, dword ptr[esp + 0x08]", localEip );
		pid = processid;
		wsprintf( cmd, L"cmp dword ptr[edx + 0x08], 0x%08X", pid );
		ASSEMBLE_AND_STEP( cmd, localEip );
		ASSEMBLER_SET_LABEL( "processParentPatch", localEip );
		wsprintf( cmd, L"jz 0x%08X", codeBase + localEip );
		ASSEMBLE_AND_STEP( cmd, localEip );

		// Return and clean the stack
		ASSEMBLE_AND_STEP( L"ret 0x08", localEip );

		// Call original function once again in order to skip the protected process
		ASSEMBLER_SET_LABEL( "processSkipLabel", localEip );
		ASSEMBLE_AND_STEP( L"mov eax, esp", localEip );
		ASSEMBLE_AND_STEP( L"add eax, 0x04", localEip );
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
		ASSEMBLE_AND_STEP( L"ret", localEip );

		// Change parent :)
		ASSEMBLER_SET_LABEL( "processParentLabel", localEip );
		std::vector<DWORD> procIdList;
		localGetProcIdList( L"explorer.exe", procIdList );
		DWORD pidOfExplorer = 0;
		if( procIdList.size() > 0 ) pidOfExplorer = procIdList[0];
		wsprintf( cmd, L"mov dword ptr[edx + 0x18], 0x%08X", pidOfExplorer );
		ASSEMBLE_AND_STEP( cmd, localEip );
		ASSEMBLE_AND_STEP( L"ret 0x08", localEip );

		// Resolve addresses which only known at the end
		patchEip = ASSEMBLER_GET_LABEL( "processNoSkipPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "processNoSkipLabel" );
		wsprintf( cmd, L"jnz 0x%08X", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "processSkipPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "processSkipLabel" );
		wsprintf( cmd, L"call 0x%08X", codeBase + labelEip );
		ASSEMBLE( cmd, patchEip );

		patchEip = ASSEMBLER_GET_LABEL( "processParentPatch" );
		labelEip = ASSEMBLER_GET_LABEL( "processParentLabel" );
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
