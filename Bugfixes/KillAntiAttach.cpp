#include "stdafx.h"
#include "..\OllyExtLocal.h"
#include "..\OllyExtIntegrity.h"
#include "KillAntiAttach.h"


typedef NTSTATUS ( WINAPI* DBGUIISSUEREMOTEBREAKIN )
(
	HANDLE hProcess
);


static bool g_applied = false;
static DBGUIISSUEREMOTEBREAKIN DbgUiIssueRemoteBreakinTrampoline = NULL;


NTSTATUS WINAPI DbgUiIssueRemoteBreakinHook( HANDLE hProcess )
{
	integrityRestore( hProcess, L"ntdll.dll" );

	return DbgUiIssueRemoteBreakinTrampoline( hProcess );
}


void killAntiAttachApply( bool protect )
{
	if( protect )
	{
		if( g_applied ) return;

		LONG beginResult = DetourTransactionBegin();
		PLUGIN_CHECK_ERROR( beginResult == NO_ERROR, return;, UNABLE_TO_START_DETOURS_TRANSACTION, beginResult );

		LONG updateThreadResult = DetourUpdateThread( GetCurrentThread() );
		PLUGIN_CHECK_ERROR( updateThreadResult == NO_ERROR, return;, UNABLE_TO_UPDATE_DETOURS_THREAD, updateThreadResult );

		DbgUiIssueRemoteBreakinTrampoline = (DBGUIISSUEREMOTEBREAKIN)localGetProcAddress( L"ntdll.dll", "DbgUiIssueRemoteBreakin" );
		if( !DbgUiIssueRemoteBreakinTrampoline ) return;

		LONG attachResult = DetourAttach( &(PVOID&)DbgUiIssueRemoteBreakinTrampoline, DbgUiIssueRemoteBreakinHook );
		PLUGIN_CHECK_ERROR( attachResult == NO_ERROR, return;, UNABLE_TO_ATTACH_DETOURS, DbgUiIssueRemoteBreakinTrampoline, attachResult );

		LONG commitResult = DetourTransactionCommit();
		PLUGIN_CHECK_ERROR( commitResult == NO_ERROR, return;, UNABLE_TO_COMMIT_DETOURS_TRANSACTION, commitResult );

		g_applied = true;
	}
	else
	{
		if( !g_applied ) return;

		LONG beginResult = DetourTransactionBegin();
		PLUGIN_CHECK_ERROR( beginResult == NO_ERROR, return;, UNABLE_TO_START_DETOURS_TRANSACTION, beginResult );

		LONG updateThreadResult = DetourUpdateThread( GetCurrentThread() );
		PLUGIN_CHECK_ERROR( updateThreadResult == NO_ERROR, return;, UNABLE_TO_UPDATE_DETOURS_THREAD, updateThreadResult );

		LONG detachResult = DetourDetach( &(PVOID&)DbgUiIssueRemoteBreakinTrampoline, DbgUiIssueRemoteBreakinHook );
		PLUGIN_CHECK_ERROR( detachResult == NO_ERROR, return;, UNABLE_TO_DETACH_DETOURS, DbgUiIssueRemoteBreakinTrampoline, detachResult );

		LONG commitResult = DetourTransactionCommit();
		PLUGIN_CHECK_ERROR( commitResult == NO_ERROR, return;, UNABLE_TO_COMMIT_DETOURS_TRANSACTION, commitResult );

		g_applied = false;
	}
}
