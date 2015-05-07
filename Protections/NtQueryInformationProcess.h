#pragma once


void ntQueryInformationProcessReset( void );
void ntQueryInformationProcessApply
(
	bool checkRemoteDebuggerPresent,
	bool processDebugObjectHandle,
	bool debugProcessFlags,
	bool parentProcess
);
