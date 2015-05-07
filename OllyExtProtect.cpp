#include "stdafx.h"
#include "OllyExtProtect.h"

#include "IsDebuggerPresent.h"
#include "NtGlobalFlag.h"
#include "HeapFlags.h"
#include "ForceFlags.h"
#include "NtQueryInformationProcess.h"
#include "OutputDebugStringA.h"
#include "OutputDebugStringW.h"
#include "CloseHandle.h"
#include "NtOpenProcess.h"
#include "BlockInput.h"
#include "TerminateProcess.h"
#include "SetInformationThread.h"
#include "NtQueryObject.h"
#include "FindWindowA.h"
#include "FindWindowExA.h"
#include "FindWindowW.h"
#include "FindWindowExW.h"

#include "Process32First.h"
#include "Process32FirstW.h"
#include "Process32Next.h"
#include "Process32NextW.h"
#include "GetTickCount.h"
#include "TimeGetTime.h"
#include "QueryPerformanceCounter.h"
#include "ZwGetContextThread.h"
#include "NtSetContextThread.h"
#include "NtQuerySystemInformation.h"
#include "NtSetDebugFilterState.h"
#include "NtSetDebugFilterState.h"
#include "KiUserExceptionDispatcher.h"
#include "ZwContinue.h"
#include "DbgPrompt.h"
#include "CreateThread.h"


sProtectOptions g_protectOptions = { 0 };


#define ISDEBUGGERPRESENT L"IsDebuggerPresent"
#define NTGLOBALFLAG L"NtGlobalFlag"
#define HEAPFLAGS L"HeapFlags"
#define FORCEFLAGS L"ForceFlags"
#define CHECKREMOTEDEBUGGERPRESENT L"CheckRemoteDebuggerPresent"
#define OUTPUTDEBUGSTRING L"OutputDebugString"
#define CLOSEHANDLE L"CloseHandle"
#define SEDEBUGPRIVILEGE L"SeDebugPrivilege"
#define BLOCKINPUT L"BlockInput"
#define PROCESSDEBUGFLAGS L"ProcessDebugFlags"
#define PROCESSDEBUGOBJECTHANDLE L"ProcessDebugObjectHandle"
#define TERMINATEPROCESS L"TerminateProcess"
#define NTSETINFORMATIONTHREAD L"NtSetInformationThread"
#define NTQUERYOBJECT L"NtQueryObject"
#define FINDWINDOW L"FindWindow"
#define NTOPENPROCESS L"NtOpenProcess"
#define PARENTPROCESS L"ParentProcess"

#define PROCESS32FIRST L"Process32First"
#define PROCESS32NEXT L"Process32Next"
#define GETTICKCOUNT L"GetTickCount"
#define TIMEGETTIME L"TimeGetTime"
#define QUERYPERFORMANCECOUNTER L"QueryPerformanceCounter"
#define ZWGETCONTEXTTHREAD L"ZwGetContextThread"
#define NTSETCONTEXTTHREAD L"NtSetContextThread"
#define KDDEBUGGERNOTPRESENT L"KdDebuggerNotPresent"
#define KDDEBUGGERENABLED L"KdDebuggerEnabled"
#define NTSETDEBUGFILTERSTATE L"NtSetDebugFilterState"
#define PROTECTDRX L"ProtectDRX"
#define HIDEDRX L"HideDRX"
#define DBGPROMPT L"DbgPrompt"
#define CREATETHREAD L"CreateThread"

#define GETFROMINI( feature, option ) \
{ \
	DWORD data = 0; \
	Getfromini( NULL, PLUGIN_NAME, feature, L"%d", &data );\
	option = ( data != 0 ); \
}


void protectReadOptions( void )
{
	GETFROMINI( ISDEBUGGERPRESENT, g_protectOptions.isDebuggerPresent );
	GETFROMINI( NTGLOBALFLAG, g_protectOptions.ntGlobalFlag );
	GETFROMINI( HEAPFLAGS, g_protectOptions.heapFlags );
	GETFROMINI( FORCEFLAGS, g_protectOptions.forceFlags );
	GETFROMINI( CHECKREMOTEDEBUGGERPRESENT, g_protectOptions.checkRemoteDebuggerPresent );
	GETFROMINI( OUTPUTDEBUGSTRING, g_protectOptions.outputDebugString );
	GETFROMINI( CLOSEHANDLE, g_protectOptions.closeHandle );
	GETFROMINI( SEDEBUGPRIVILEGE, g_protectOptions.seDebugPrivilege );
	GETFROMINI( BLOCKINPUT, g_protectOptions.blockInput );
	GETFROMINI( PROCESSDEBUGFLAGS, g_protectOptions.processDebugFlags );
	GETFROMINI( PROCESSDEBUGOBJECTHANDLE, g_protectOptions.processDebugObjectHandle );
	GETFROMINI( TERMINATEPROCESS, g_protectOptions.terminateProcess );
	GETFROMINI( NTSETINFORMATIONTHREAD, g_protectOptions.ntSetInformationThread );
	GETFROMINI( NTQUERYOBJECT, g_protectOptions.ntQueryObject );
	GETFROMINI( FINDWINDOW, g_protectOptions.findWindow );
	GETFROMINI( NTOPENPROCESS, g_protectOptions.ntOpenProcess );

	GETFROMINI( PROCESS32FIRST, g_protectOptions.process32First );
	GETFROMINI( PROCESS32NEXT, g_protectOptions.process32Next );
	GETFROMINI( PARENTPROCESS, g_protectOptions.parentProcess );
	GETFROMINI( GETTICKCOUNT, g_protectOptions.getTickCount );
	GETFROMINI( TIMEGETTIME, g_protectOptions.timeGetTime );
	GETFROMINI( QUERYPERFORMANCECOUNTER, g_protectOptions.queryPerformanceCounter );
	GETFROMINI( ZWGETCONTEXTTHREAD, g_protectOptions.zwGetContextThread );
	GETFROMINI( NTSETCONTEXTTHREAD, g_protectOptions.ntSetContextThread );
	GETFROMINI( KDDEBUGGERNOTPRESENT, g_protectOptions.kdDebuggerNotPresent );
	GETFROMINI( KDDEBUGGERENABLED, g_protectOptions.kdDebuggerEnabled );
	GETFROMINI( NTSETDEBUGFILTERSTATE, g_protectOptions.ntSetDebugFilterState );
	GETFROMINI( PROTECTDRX, g_protectOptions.protectDRX );
	GETFROMINI( HIDEDRX, g_protectOptions.hideDRX );
	GETFROMINI( DBGPROMPT, g_protectOptions.dbgPrompt );
	GETFROMINI( CREATETHREAD, g_protectOptions.createThread );
}


void protectWriteOptions( void )
{
	Writetoini( NULL, PLUGIN_NAME, ISDEBUGGERPRESENT, L"%d", g_protectOptions.isDebuggerPresent );
	Writetoini( NULL, PLUGIN_NAME, NTGLOBALFLAG, L"%d", g_protectOptions.ntGlobalFlag );
	Writetoini( NULL, PLUGIN_NAME, HEAPFLAGS, L"%d", g_protectOptions.heapFlags );
	Writetoini( NULL, PLUGIN_NAME, FORCEFLAGS, L"%d", g_protectOptions.forceFlags );
	Writetoini( NULL, PLUGIN_NAME, CHECKREMOTEDEBUGGERPRESENT, L"%d", g_protectOptions.checkRemoteDebuggerPresent );
	Writetoini( NULL, PLUGIN_NAME, OUTPUTDEBUGSTRING, L"%d", g_protectOptions.outputDebugString );
	Writetoini( NULL, PLUGIN_NAME, CLOSEHANDLE, L"%d", g_protectOptions.closeHandle );
	Writetoini( NULL, PLUGIN_NAME, SEDEBUGPRIVILEGE, L"%d", g_protectOptions.seDebugPrivilege );
	Writetoini( NULL, PLUGIN_NAME, BLOCKINPUT, L"%d", g_protectOptions.blockInput );
	Writetoini( NULL, PLUGIN_NAME, PROCESSDEBUGFLAGS, L"%d", g_protectOptions.processDebugFlags );
	Writetoini( NULL, PLUGIN_NAME, PROCESSDEBUGOBJECTHANDLE, L"%d", g_protectOptions.processDebugObjectHandle );
	Writetoini( NULL, PLUGIN_NAME, TERMINATEPROCESS, L"%d", g_protectOptions.terminateProcess );
	Writetoini( NULL, PLUGIN_NAME, NTSETINFORMATIONTHREAD, L"%d", g_protectOptions.ntSetInformationThread );
	Writetoini( NULL, PLUGIN_NAME, NTQUERYOBJECT, L"%d", g_protectOptions.ntQueryObject );
	Writetoini( NULL, PLUGIN_NAME, FINDWINDOW, L"%d", g_protectOptions.findWindow );
	Writetoini( NULL, PLUGIN_NAME, NTOPENPROCESS, L"%d", g_protectOptions.ntOpenProcess );

	Writetoini( NULL, PLUGIN_NAME, PROCESS32FIRST, L"%d", g_protectOptions.process32Next );
	Writetoini( NULL, PLUGIN_NAME, PROCESS32NEXT, L"%d", g_protectOptions.process32Next );
	Writetoini( NULL, PLUGIN_NAME, PARENTPROCESS, L"%d", g_protectOptions.parentProcess );
	Writetoini( NULL, PLUGIN_NAME, GETTICKCOUNT, L"%d", g_protectOptions.getTickCount );
	Writetoini( NULL, PLUGIN_NAME, TIMEGETTIME, L"%d", g_protectOptions.timeGetTime );
	Writetoini( NULL, PLUGIN_NAME, QUERYPERFORMANCECOUNTER, L"%d", g_protectOptions.queryPerformanceCounter );
	Writetoini( NULL, PLUGIN_NAME, ZWGETCONTEXTTHREAD, L"%d", g_protectOptions.zwGetContextThread );
	Writetoini( NULL, PLUGIN_NAME, NTSETCONTEXTTHREAD, L"%d", g_protectOptions.ntSetContextThread );
	Writetoini( NULL, PLUGIN_NAME, KDDEBUGGERNOTPRESENT, L"%d", g_protectOptions.kdDebuggerNotPresent );
	Writetoini( NULL, PLUGIN_NAME, KDDEBUGGERENABLED, L"%d", g_protectOptions.kdDebuggerEnabled );
	Writetoini( NULL, PLUGIN_NAME, NTSETDEBUGFILTERSTATE, L"%d", g_protectOptions.ntSetDebugFilterState );
	Writetoini( NULL, PLUGIN_NAME, PROTECTDRX, L"%d", g_protectOptions.protectDRX );
	Writetoini( NULL, PLUGIN_NAME, HIDEDRX, L"%d", g_protectOptions.hideDRX );
	Writetoini( NULL, PLUGIN_NAME, DBGPROMPT, L"%d", g_protectOptions.dbgPrompt );
	Writetoini( NULL, PLUGIN_NAME, CREATETHREAD, L"%d", g_protectOptions.createThread );
}


void protectPreHook( void )
{
	ntGlobalFlagPreHook( g_protectOptions.ntGlobalFlag );
}


void protectPostHook( void )
{
	ntGlobalFlagPostHook( g_protectOptions.ntGlobalFlag );
}


void protectReset( void )
{
	isDebuggerPresentReset();
	ntGlobalFlagReset();
	heapFlagsReset();
	forceFlagsReset();
	ntQueryInformationProcessReset();
	outputDebugStringAReset();
	outputDebugStringWReset();
	closeHandleReset();
	ntOpenProcessReset();
	blockInputReset();
	terminateProcessReset();
	setInformationThreadReset();
	ntQueryObjectReset();
	findWindowAReset();
	findWindowExAReset();
	findWindowWReset();
	findWindowExWReset();

	process32FirstReset();
	process32FirstWReset();
	process32NextReset();
	process32NextWReset();
	getTickCountReset();
	timeGetTimeReset();
	queryPerformanceCounterReset();
	zwGetContextThreadReset();
	ntSetContextThreadReset();
	ntQuerySystemInformationReset();
	ntSetDebugFilterStateReset();
	kiUserExceptionDispatcherReset();
	zwContinueReset();
	dbgPromptReset();
	createThreadReset();
}


bool protectApply( void )
{
	PLUGIN_CHECK_ERROR( process, return false;, NO_DEBUGGE );

	isDebuggerPresentApply( g_protectOptions.isDebuggerPresent );
	ntGlobalFlagApply( g_protectOptions.ntGlobalFlag );
	heapFlagsApply( g_protectOptions.heapFlags );
	forceFlagsApply( g_protectOptions.forceFlags );
	ntQueryInformationProcessApply
	(
		g_protectOptions.checkRemoteDebuggerPresent,
		g_protectOptions.processDebugFlags,
		g_protectOptions.processDebugObjectHandle,
		g_protectOptions.parentProcess
	);
	outputDebugStringAApply( g_protectOptions.outputDebugString );
	outputDebugStringWApply( g_protectOptions.outputDebugString );
	closeHandleApply( g_protectOptions.closeHandle );
	ntOpenProcessApply
	(
		g_protectOptions.seDebugPrivilege,
		g_protectOptions.ntOpenProcess
	);
	blockInputApply( g_protectOptions.blockInput );
	terminateProcessApply( g_protectOptions.terminateProcess );
	setInformationThreadApply( g_protectOptions.ntSetInformationThread );
	ntQueryObjectApply( g_protectOptions.ntQueryObject );
	findWindowAApply( g_protectOptions.findWindow );
	findWindowExAApply( g_protectOptions.findWindow );
	findWindowWApply( g_protectOptions.findWindow );
	findWindowExWApply( g_protectOptions.findWindow );

	process32FirstApply( g_protectOptions.process32First );
	process32FirstWApply( g_protectOptions.process32First );
	process32NextApply( g_protectOptions.process32Next );
	process32NextWApply( g_protectOptions.process32Next );
	getTickCountApply( g_protectOptions.getTickCount );
	timeGetTimeApply( g_protectOptions.timeGetTime );
	queryPerformanceCounterApply( g_protectOptions.queryPerformanceCounter );
	zwGetContextThreadApply( g_protectOptions.zwGetContextThread );
	ntSetContextThreadApply( g_protectOptions.ntSetContextThread );
	ntQuerySystemInformationApply
	(
		g_protectOptions.kdDebuggerNotPresent,
		g_protectOptions.kdDebuggerEnabled
	);
	ntSetDebugFilterStateApply( g_protectOptions.ntSetDebugFilterState );
	kiUserExceptionDispatcherApply
	(
		g_protectOptions.protectDRX,
		g_protectOptions.hideDRX
	);
	zwContinueApply( g_protectOptions.protectDRX );
	dbgPromptApply( g_protectOptions.dbgPrompt );
	createThreadApply( g_protectOptions.createThread );

	Redrawcpudisasm();

	return true;
}
