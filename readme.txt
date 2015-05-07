OllyExt is a plugin for Olly 2.xx debugger.

The main intention of this plugin is to provide the biggest anti-anti debugging features
and bugfixes for Olly 2.xx. Updates will come... :)

VMProtect support!

The currently available commands are the following:
	- Code Rip to Clipboard
	- Code Rip to Clipboard Recursive
	- Data Rip to Clipboard

The currently supported protections are the following:
	- IsDebuggerPresent
	- NtGlobalFlag
	- HeapFlag
	- ForceFlag
	- CheckRemoteDebuggerPresent
	- OutputDebugString
	- CloseHandle
	- SeDebugPrivilege
	- BlockInput
	- ProcessDebugFlags
	- ProcessDebugObjectHandle
	- TerminateProcess
	- NtSetInformationThread
	- NtQueryObject
	- FindWindow
	- NtOpenProcess
	- Process32First
	- Process32Next
	- ParentProcess
	- GetTickCount
	- timeGetTime
	- QueryPerformanceCounter
	- ZwGetContextThread
	- NtSetContextThread
	- KdDebuggerNotPresent
	- KdDebuggerEnabled
	- NtSetDebugFilterState
	- ProtectDRX
	- HideDRX
	- DbgPrompt
	- CreateThread

The currently supported bugfixes are the following:
	- Caption change
	- Kill Anti-Attach ( dll integrity check )

Requirements:
 - Microsoft Visual C++ 2010 Redistributable Package (x86)

OS support:
 - WinXP x32
 - WinXP WoW64
 - Win7 x32
 - Win7 WoW64

Limitations:
 - Because of missing PDK function data ripping is ONLY on 2.01 latest supported

If you have any problem just notify me.

About the author:

Created by Ferrit
Send your bugreports/comments to ferrit.rce@gmail.com

Enjoy :P
