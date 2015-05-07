#include "stdafx.h"
#include "..\OllyExtLocal.h"
#include "..\OllyExtHook.h"
#include "..\OllyExtAssembler.h"
#include "NtQueryInformationProcess.h"


#define ASSEMBLER_ERROR_HANDLER() \
	delete [] localHookProc; \
	hookFree( process, LIBNAME, PROCNAME, g_hook ); \
	return;

#define IS_AT_LEAST_ONE_PROTECTION_ENABLED() \
	( checkRemoteDebuggerPresent || \
		debugProcessFlags || \
		processDebugObjectHandle || \
		parentProcess )

#define IS_CONFIG_CHANGED() \
	( checkRemoteDebuggerPresent != g_checkRemoteDebuggerPresent || \
		debugProcessFlags != g_debugProcessFlags || \
		processDebugObjectHandle != g_processDebugObjectHandle || \
		parentProcess != g_parentProcess )

#define LIBNAME ( L"ntdll.dll" )
#define PROCNAME ( "NtQueryInformationProcess" )
#define HOOK_SIZE ( 0x100 )


static bool g_applied = false;
static bool g_checkRemoteDebuggerPresent = false;
static bool g_debugProcessFlags = false;
static bool g_processDebugObjectHandle = false;
static bool g_parentProcess = false;
static sHook g_hook = { 0 };


void ntQueryInformationProcessReset( void )
{
	g_applied = false;
}


void ntQueryInformationProcessApply
(
	bool checkRemoteDebuggerPresent,
	bool processDebugObjectHandle,
	bool debugProcessFlags,
	bool parentProcess
)
{
	if( IS_AT_LEAST_ONE_PROTECTION_ENABLED() )
	{
		// If it's already applied but the config not changed just return
		if( g_applied &&
			!IS_CONFIG_CHANGED() ) return;

		// Here we have 2 chances
		// 1. Hook not applied -> just apply it
		// 2. Hook applied and config changed -> remove old hook and apply new one
		if( g_applied )
		{
			wchar_t procNameW[TEXTLEN];
			Asciitounicode( PROCNAME, strlen( PROCNAME ), procNameW, TEXTLEN );

			bool freeResult = hookFree( process, LIBNAME, PROCNAME, g_hook );
			PLUGIN_CHECK_ERROR( freeResult, return;, UNABLE_TO_FREE_HOOK, LIBNAME, procNameW );

			g_applied = false;
		}

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
		ASSEMBLE_AND_STEP( L"mov ecx, 0x05", localEip );
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

		if( checkRemoteDebuggerPresent )
		{
			// If ProcessInformationClass == 0x07
			ASSEMBLE_AND_STEP( L"cmp dword ptr[esp + 0x08], 0x07", localEip );
			ASSEMBLER_SET_LABEL( "notPortPatch", localEip );
			wsprintf( cmd, L"jnz 0x%08X", codeBase + localEip );
			ASSEMBLE_AND_STEP( cmd, localEip );

			// Remove debug port :)
			ASSEMBLE_AND_STEP( L"mov edx, [esp + 0x0C]", localEip );
			ASSEMBLE_AND_STEP( L"mov dword ptr[edx], 0x00", localEip );
		}

		ASSEMBLER_SET_LABEL( "notPortLabel", localEip );

		if( debugProcessFlags )
		{
			// If ProcessInformationClass == 0x1F
			ASSEMBLE_AND_STEP( L"cmp dword ptr[esp + 0x08], 0x1F", localEip );
			ASSEMBLER_SET_LABEL( "notFlagsPatch", localEip );
			wsprintf( cmd, L"jnz 0x%08X", codeBase + localEip );
			ASSEMBLE_AND_STEP( cmd, localEip );

			// Remove debug flags :)
			ASSEMBLE_AND_STEP( L"mov edx, [esp + 0x0C]", localEip );
			ASSEMBLE_AND_STEP( L"mov dword ptr[edx], 0x01", localEip );
		}

		ASSEMBLER_SET_LABEL( "notFlagsLabel", localEip );

		if( processDebugObjectHandle )
		{
			// If ProcessInformationClass == 0x1E
			ASSEMBLE_AND_STEP( L"cmp dword ptr[esp + 0x08], 0x1E", localEip );
			ASSEMBLER_SET_LABEL( "notObjectPatch", localEip );
			wsprintf( cmd, L"jnz 0x%08X", codeBase + localEip );
			ASSEMBLE_AND_STEP( cmd, localEip );

			// Remove debug flags :)
			ASSEMBLE_AND_STEP( L"mov edx, [esp + 0x0C]", localEip );
			ASSEMBLE_AND_STEP( L"mov dword ptr[edx], 0x00", localEip );

			// Return with STATUS_PORT_NOT_SET
			wsprintf( cmd, L"mov eax, 0x%08X", 0xC0000353 );
			ASSEMBLE_AND_STEP( cmd, localEip );
		}

		ASSEMBLER_SET_LABEL( "notObjectLabel", localEip );

		if( parentProcess )
		{
			// If ProcessInformationClass == 0x00
			ASSEMBLE_AND_STEP( L"cmp dword ptr[esp + 0x08], 0x00", localEip );
			ASSEMBLER_SET_LABEL( "notParentPatch", localEip );
			wsprintf( cmd, L"jnz 0x%08X", codeBase + localEip );
			ASSEMBLE_AND_STEP( cmd, localEip );

			// Change parent :)
			ASSEMBLE_AND_STEP( L"mov edx, [esp + 0x0C]", localEip );
			std::vector<DWORD> procIdList;
			localGetProcIdList( L"explorer.exe", procIdList );
			DWORD pidOfExplorer = 0;
			if( procIdList.size() > 0 ) pidOfExplorer = procIdList[0];
			wsprintf( cmd, L"mov dword ptr[edx + 0x14], 0x%08X", pidOfExplorer );
			ASSEMBLE_AND_STEP( cmd, localEip );
		}

		ASSEMBLER_SET_LABEL( "notParentLabel", localEip );

		// Return and clean the stack
		ASSEMBLE_AND_STEP( L"ret 0x14", localEip );

		// Resolve addresses which only known at the end
		patchEip = ASSEMBLER_GET_LABEL( "notPortPatch" );
		if( patchEip )
		{
			labelEip = ASSEMBLER_GET_LABEL( "notPortLabel" );
			wsprintf( cmd, L"jnz 0x%08X", codeBase + labelEip );
			ASSEMBLE( cmd, patchEip );
		}
		patchEip = ASSEMBLER_GET_LABEL( "notFlagsPatch" );
		if( patchEip )
		{
			labelEip = ASSEMBLER_GET_LABEL( "notFlagsLabel" );
			wsprintf( cmd, L"jnz 0x%08X", codeBase + labelEip );
			ASSEMBLE( cmd, patchEip );
		}
		patchEip = ASSEMBLER_GET_LABEL( "notObjectPatch" );
		if( patchEip )
		{
			labelEip = ASSEMBLER_GET_LABEL( "notObjectLabel" );
			wsprintf( cmd, L"jnz 0x%08X", codeBase + labelEip );
			ASSEMBLE( cmd, patchEip );
		}
		patchEip = ASSEMBLER_GET_LABEL( "notParentPatch" );
		if( patchEip )
		{
			labelEip = ASSEMBLER_GET_LABEL( "notParentLabel" );
			wsprintf( cmd, L"jnz 0x%08X", codeBase + labelEip );
			ASSEMBLE( cmd, patchEip );
		}

		ASSEMBLER_FLUSH();

		bool attachResult = hookAttach( process, LIBNAME, PROCNAME, localHookProc, g_hook );
		PLUGIN_CHECK_ERROR( attachResult,
			delete [] localHookProc; \
			hookFree( process, LIBNAME, PROCNAME, g_hook ); \
			return;,
			UNABLE_TO_ATTACH_HOOK, LIBNAME, procNameW );

		delete [] localHookProc;

		g_debugProcessFlags = debugProcessFlags;
		g_checkRemoteDebuggerPresent = checkRemoteDebuggerPresent;
		g_processDebugObjectHandle = processDebugObjectHandle;
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
