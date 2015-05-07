#include "stdafx.h"
#include "OllyExtRemote.h"
#include "OllyExtHook.h"


#define UNCONDITIONAL_JMP_SIZE ( 5 )


bool hookAlloc( HANDLE hProcess, wchar_t* libName, char* procName, sHook& rHook )
{
	BOOL writeResult = FALSE;
	wchar_t cmd[64] = { 0 };
	DWORD codeLen = 0;
	wchar_t assemblyError[TEXTLEN] = { 0 };

	// Get API address
	rHook.remoteTargetProc = remoteGetProcAddress( hProcess, libName, procName );
	if( !rHook.remoteTargetProc )
		return false;

	// Get number of bytes has to be saved into trampoline
	DWORD apiOrigCodeSize = 0;
	DWORD ptr = (DWORD)rHook.remoteTargetProc;
	BYTE buffer[MAXCMDSIZE];
	while( apiOrigCodeSize < UNCONDITIONAL_JMP_SIZE )
	{		
		DISASM da = { 0 };

		// Read the assembly from the target binary
		PLUGIN_CHECK_ERROR( Readmemory( buffer, ptr, MAXCMDSIZE, MM_SILENT ),
			return false;,
			UNABLE_TO_READ_REMOTE_PROCESS_MEMORY, ptr, MAXCMDSIZE );

		// Reverse the code
		da.EIP = (DWORD)buffer;
		da.VirtualAddr = ptr;
		DWORD cmdSize = BeaEngineDisasm( &da );
		PLUGIN_CHECK_ERROR( cmdSize != OUT_OF_BLOCK && cmdSize != UNKNOWN_OPCODE,
			return false;,
			UNABLE_TO_DISASSEMBLE_CODE, da.VirtualAddr );

		ptr += cmdSize;
		apiOrigCodeSize += cmdSize;
	}

	// Calculate trampoline proc size
	rHook.trampolineProcSize = apiOrigCodeSize + UNCONDITIONAL_JMP_SIZE;

	// Allocate trampoline proc
	rHook.trampolineProc = VirtualAllocEx( hProcess, NULL, rHook.trampolineProcSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
	PLUGIN_CHECK_ERROR( rHook.trampolineProc, return false;, UNABLE_TO_ALLOCATE_REMOTE_MEMORY );

	// Copy the original data from target API function to trampoline
	//writeResult = WriteProcessMemory( hProcess, rHook.trampolineProc, rHook.remoteTargetProc, apiOrigCodeSize, NULL );
	Readmemory(buffer, (DWORD)rHook.remoteTargetProc, MAXCMDSIZE, MM_SILENT);
	writeResult = WriteProcessMemory(hProcess, rHook.trampolineProc, buffer, apiOrigCodeSize, NULL);
	PLUGIN_CHECK_ERROR( writeResult,
		VirtualFreeEx( hProcess, rHook.trampolineProc, 0, MEM_RELEASE ); \
		return false;,
		UNABLE_TO_WRITE_MEMORY_AT, rHook.trampolineProc );

	// Assemble an unconditional jump back to the target API function
	BYTE jmpToOrigFunc[UNCONDITIONAL_JMP_SIZE] = { 0 };
	wsprintf( cmd, L"jmp 0x%08X", (DWORD)rHook.remoteTargetProc + apiOrigCodeSize );
	codeLen = Assemble( cmd, (DWORD)rHook.trampolineProc + apiOrigCodeSize, jmpToOrigFunc, UNCONDITIONAL_JMP_SIZE, 0, assemblyError );
	PLUGIN_CHECK_ERROR( codeLen,
		VirtualFreeEx( hProcess, rHook.trampolineProc, 0, MEM_RELEASE ); \
		return false;,
		UNABLE_TO_ASSEMBLE_CODE, cmd, (DWORD)(rHook.trampolineProc) + apiOrigCodeSize, assemblyError );

	// Copy this unconditional jump into trampoline
	writeResult = WriteProcessMemory( hProcess, (void*)( (DWORD)rHook.trampolineProc + apiOrigCodeSize ), jmpToOrigFunc, UNCONDITIONAL_JMP_SIZE, NULL );
	PLUGIN_CHECK_ERROR( writeResult,
		VirtualFreeEx( hProcess, rHook.trampolineProc, 0, MEM_RELEASE ); \
		return false;,
		UNABLE_TO_WRITE_MEMORY_AT, (DWORD)rHook.trampolineProc + apiOrigCodeSize );

	// Allocate hook proc
	rHook.remoteHookProc = VirtualAllocEx( hProcess, NULL, rHook.hookSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
	PLUGIN_CHECK_ERROR( rHook.remoteHookProc,
		VirtualFreeEx( hProcess, rHook.trampolineProc, 0, MEM_RELEASE ); \
		return false;,
		UNABLE_TO_ALLOCATE_REMOTE_MEMORY );

	return true;
}


bool hookAttach( HANDLE hProcess, wchar_t* libName, char* procName, void* localHookProc, sHook& rHook )
{
	BOOL writeResult = FALSE;
	wchar_t cmd[64] = { 0 };
	DWORD codeLen = 0;
	wchar_t assemblyError[TEXTLEN] = { 0 };

	// Copy the hook function into the target process
	writeResult = WriteProcessMemory( hProcess, rHook.remoteHookProc, localHookProc, rHook.hookSize, NULL );
	PLUGIN_CHECK_ERROR( writeResult, return false;, UNABLE_TO_WRITE_MEMORY_AT, rHook.remoteHookProc );

	// Assemble an unconditional jump to the hook function
	BYTE jmpToHookFunc[UNCONDITIONAL_JMP_SIZE] = { 0 };
	wsprintf( cmd, L"jmp 0x%08X", rHook.remoteHookProc );
	codeLen = Assemble( cmd, (DWORD)rHook.remoteTargetProc, jmpToHookFunc, UNCONDITIONAL_JMP_SIZE, 0, assemblyError );
	PLUGIN_CHECK_ERROR( codeLen, return false;, UNABLE_TO_ASSEMBLE_CODE, cmd, rHook.remoteHookProc, assemblyError );

	// Copy this unconditional jump into target API function
	writeResult = WriteProcessMemory( hProcess, rHook.remoteTargetProc, jmpToHookFunc, UNCONDITIONAL_JMP_SIZE, NULL );
	PLUGIN_CHECK_ERROR( writeResult, return false;, UNABLE_TO_WRITE_MEMORY_AT, rHook.remoteTargetProc );

	return true;
}


bool hookFree( HANDLE hProcess, wchar_t* libName, char* procName, sHook& rHook )
{
	BOOL readResult = FALSE;
	BOOL writeResult = FALSE;
	BOOL freeResult = FALSE;

	if( rHook.trampolineProcSize &&
		rHook.trampolineProc )
	{
		void* localTrampolineProc = new BYTE[rHook.trampolineProcSize];
		PLUGIN_CHECK_ERROR( localTrampolineProc, return false;, UNABLE_TO_ALLOCATE_LOCAL_MEMORY );

		// Copy the trampoline to a local buffer
		readResult = ReadProcessMemory( hProcess, rHook.trampolineProc, localTrampolineProc, rHook.trampolineProcSize, NULL );
		PLUGIN_CHECK_ERROR( readResult,
			delete [] localTrampolineProc; \
			return false;,
			UNABLE_TO_READ_MEMORY_AT, rHook.trampolineProc );

		// Check API function
		if( rHook.remoteTargetProc )
		{
			// Copy the saved trampoline instructions back to target API function
			writeResult = WriteProcessMemory( hProcess, rHook.remoteTargetProc, localTrampolineProc, UNCONDITIONAL_JMP_SIZE, NULL );
			PLUGIN_CHECK_ERROR( writeResult,
				delete [] localTrampolineProc; \
				return false;,
				UNABLE_TO_WRITE_MEMORY_AT, rHook.remoteTargetProc );
		}

		delete [] localTrampolineProc;
		localTrampolineProc = NULL;
	}

	// Free hook proc
	if( rHook.remoteHookProc )
	{
		freeResult = VirtualFreeEx( hProcess, rHook.remoteHookProc, 0, MEM_RELEASE );
		PLUGIN_CHECK_ERROR( freeResult, return false;, UNABLE_TO_FREE_REMOTE_MEMORY, rHook.remoteHookProc );
	}

	// Free trampoline proc
	if( rHook.trampolineProc )
	{
		freeResult = VirtualFreeEx( hProcess, rHook.trampolineProc, 0, MEM_RELEASE );
		PLUGIN_CHECK_ERROR( freeResult, return false;, UNABLE_TO_FREE_REMOTE_MEMORY, rHook.trampolineProc );
	}

	memset( &rHook, 0, sizeof( sHook ) );

	return true;
}
