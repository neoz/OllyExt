#pragma once


typedef struct
{
	bool isDebuggerPresent;
	bool ntGlobalFlag;
	bool heapFlags;
	bool forceFlags;
	bool checkRemoteDebuggerPresent;
	bool outputDebugString;
	bool closeHandle;
	bool seDebugPrivilege;
	bool blockInput;
	bool processDebugFlags;
	bool processDebugObjectHandle;
	bool terminateProcess;
	bool ntSetInformationThread;
	bool ntQueryObject;
	bool findWindow;
	bool ntOpenProcess;
	bool process32First;
	bool process32Next;
	bool parentProcess;
	bool getTickCount;
	bool timeGetTime;
	bool queryPerformanceCounter;
	bool zwGetContextThread;
	bool ntSetContextThread;
	bool kdDebuggerNotPresent;
	bool kdDebuggerEnabled;
	bool ntSetDebugFilterState;
	bool protectDRX;
	bool hideDRX;
	bool dbgPrompt;
	bool createThread;
} sProtectOptions;


extern sProtectOptions g_protectOptions;


void protectReadOptions( void );
void protectWriteOptions( void );
void protectPreHook( void );
void protectPostHook( void );
void protectReset( void );
bool protectApply( void );
