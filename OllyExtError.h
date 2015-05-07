#pragma once


#define INVALID_OLLY_VERSION ( L"OllyExt only supports version v%d or higher but v%d detected" )
#define UNABLE_TO_GET_WINDOW_TEXT ( L"Unable to get window text" )
#define UNABLE_TO_SET_WINDOW_TEXT ( L"Unable to set window text" )
#define INVALID_PROCESS_ID ( L"Invalid process Id" )
#define NO_DEBUGGE ( L"No active debugge found" )
#define NO_DEBUGGE_NAME ( L"No debugge filename" )
#define UNABLE_TO_GET_REMOTE_MODULE_INFO ( L"Unable to get remote module information %s" )
#define UNABLE_TO_READ_MEMORY_AT ( L"Unable to read memory at: %08X" )
#define UNABLE_TO_WRITE_MEMORY_AT ( L"Unable to write memory at: %08X" )
#define UNABLE_TO_CHANGE_PROTECTION_AT ( L"Unable to change memory protection at: %08X" )
#define UNABLE_TO_RESTORE_PROTECTION_AT ( L"Unable to restore memory protection at: %08X" )
#define UNABLE_TO_EXECUTE_REMOTE_THREAD ( L"Unable to execute remote thread" )
#define UNABLE_TO_LOAD_MODULE ( L"Unable to load module: %s" )
#define UNABLE_TO_FIND_MODULE ( L"Unable to find module: %s" )
#define UNABLE_TO_FIND_FUNCTION ( L"Unable to find function: %s::%s" )
#define UNABLE_TO_ALLOCATE_LOCAL_MEMORY ( L"Unable to allocate memory" )
#define UNABLE_TO_ALLOCATE_REMOTE_MEMORY ( L"Unable to allocate memory in debugge" )
#define UNABLE_TO_ALLOCATE_GLOBAL_MEMORY ( L"Unable to allocate global memory" )
#define UNABLE_TO_FREE_GLOBAL_MEMORY ( L"Unable to free global memory" )
#define UNABLE_TO_FREE_REMOTE_MEMORY ( L"Unable to free memory in debugge: %08X" )
#define UNABLE_TO_ASSEMBLE_CODE ( L"Unable to assemble code: %s at: %08X error: %s" )
#define UNABLE_TO_DISASSEMBLE_CODE ( L"Unable to disassemble at: %08X" )
#define UNABLE_TO_ADD_STRING ( L"Unable to add string: %s at: %08X" )
#define UNABLE_TO_START_DETOURS_TRANSACTION ( L"Unable to start detours transaction: %08X" )
#define UNABLE_TO_UPDATE_DETOURS_THREAD ( L"Unable to update detours thread: %08X" )
#define UNABLE_TO_ATTACH_DETOURS ( L"Unable to attach detours function: %08X error: %08X" )
#define UNABLE_TO_DETACH_DETOURS ( L"Unable to detach detours function: %08X error: %08X" )
#define UNABLE_TO_COMMIT_DETOURS_TRANSACTION ( L"Unable to commit detours transaction: %08X" )
#define UNABLE_TO_ALLOC_HOOK ( L"Unable to allocate hook for module: %s func: %s" )
#define UNABLE_TO_ATTACH_HOOK ( L"Unable to attach hook for module: %s func: %s" )
#define UNABLE_TO_FREE_HOOK ( L"Unable to free hook for module: %s func: %s" )
#define UNABLE_TO_GET_MODULE_FILENAME ( L"Unable to get module file name: %08X" )
#define UNABLE_TO_OPEN_FILE ( L"Unable to open file: %s" )
#define UNABLE_TO_CREATE_FILE_MAPPING ( L"Unable to create file mapping: %08X" )
#define UNABLE_TO_MAP_FILE ( L"Unable to map file: %08X" )
#define INVALID_DOS_SIGNATURE_AT ( L"Invalid DOS signature at: %08X" )
#define INVALID_NT_SIGNATURE_AT ( L"Invalid NT signature at: %08X" )
#define INVALID_OPTIONAL_SIGNATURE_AT ( L"Invalid OPTIONAL signature at: %08X" )
#define UNABLE_TO_FIND_TEXT_SECTION_IN ( L"Unable to find text section in module: %s" )
#define UNABLE_TO_GET_PROCESS_PEB ( L"Unable to get process PEB" )
#define INVALID_WIN_VERSION ( L"OllyExt is not tested with this windows version.\r\nMaybe some protections won't work properly!\r\nSee readme.txt for supported versions." )
#define INVALID_ARCHITECTURE ( L"OllyExt is not tested with this processor architecture.\r\nMaybe some protections won't work properly!\r\nSee readme.txt for supported architectures." )
#define UNABLE_TO_OPEN_OR_CREATE_REGISTRY_KEY ( L"Unable to open or create registry key: %s error: %08X" )
#define UNABLE_TO_SET_REGISTRY_KEY ( L"Unable to set registry key: %s name: %s error: %08X" )
#define UNABLE_TO_CLOSE_REGISTRY_KEY ( L"Unable to close registry key: %s error: %08X" )
#define UNABLE_TO_COPY_DRIVER ( L"Unable to copy driver: %s" )
#define UNABLE_TO_OPEN_SCM ( L"Unable to open service control manager" )
#define UNABLE_TO_CREATE_SERVICE ( L"Unable to create service" )
#define UNABLE_TO_GET_SERVICE_STATUS ( L"Unable to get service status" )
#define UNABLE_TO_START_SERVICE ( L"Unable to start service" )
#define UNABLE_TO_STOP_SERVICE ( L"Unable to stop service" )
#define UNABLE_TO_CLOSE_SERVICE ( L"Unable to close service" )
#define UNABLE_TO_OPEN_SERVICE ( L"Unable to open service" )
#define UNABLE_TO_OPEN_DRIVER ( L"Unable to open driver" )
#define UNABLE_TO_FIND_RESOURCE ( L"Unable to find resource" )
#define UNABLE_TO_GET_RESOURCE_SIZE ( L"Unable to get resource size" )
#define UNABLE_TO_LOAD_RESOURCE ( L"Unable to load resource" )
#define UNABLE_TO_LOCK_RESOURCE ( L"Unable to lock resource" )
#define UNABLE_TO_LOCK_GLOBAL_DATA ( L"Unable to lock global data" )
#define UNABLE_TO_UNLOCK_GLOBAL_DATA ( L"Unable to unlock global data" )
#define UNABLE_TO_SAVE_FILE_BECAUSE_OF_FAILBIT ( L"Unable to save file beacause of fail bit" )
#define UNABLE_TO_SAVE_FILE_BECAUSE_OF_BADBIT ( L"Unable to save file beacause of bad bit" )
#define UNABLE_TO_GET_SYSTEM_DIRECTORY ( L"Unable to get system directory" )
#define UNABLE_TO_CONCAT_STRINGS ( L"Unable to concat strings" )
#define UNABLE_TO_UNCOMPRESS_DATA ( L"Unable to uncompress data" )
#define UNABLE_TO_SEND_CMD_TO_DRIVER ( L"Unable to send command to driver" )
#define UNABLE_TO_CLEAR_CLIPBOARD ( L"Unable to clear clipboard" )
#define UNABLE_TO_OPEN_CLIPBOARD ( L"Unable to open clipboard" )
#define UNABLE_TO_CLOSE_CLIPBOARD ( L"Unable to close clipboard" )
#define UNABLE_TO_GET_DISASSEMBLER_DUMP ( L"Unable to get disassembler window dump" )
#define UNABLE_TO_READ_REMOTE_PROCESS_MEMORY ( L"Unable to read remote process memory at %08X with size %d" )
#define UNABLE_TO_CONVERT_WCHAR_TO_CHAR ( L"Unable to convert string: %ws" )
#define UNABLE_TO_FIND_TARGET_ADDRESS ( L"Unable to find target jump address at %08X" )
#define UNABLE_TO_RIP_DATA ( L"Unable to RIP data. Only Olly2.01 version supported!" )

#define PLUGIN_CHECK_ERROR( condition, action, errorFormat, ... ) \
{ \
	if( !( condition ) ) \
	{ \
		wchar_t errorStr[TEXTLEN]; \
		wsprintf( errorStr, ( errorFormat ), __VA_ARGS__ ); \
		wchar_t placeStr[TEXTLEN]; \
		wsprintf( placeStr, L"\nFile: %s Line: %d", _T( __FILE__ ), __LINE__ ); \
		wcscat_s( errorStr, TEXTLEN, placeStr ); \
		wchar_t lastErrorStr[TEXTLEN]; \
		wsprintf( lastErrorStr, L"\nResult of GetLastError: %08X", GetLastError() ); \
		wcscat_s( errorStr, TEXTLEN, lastErrorStr ); \
		MessageBox( NULL, errorStr, PLUGIN_NAME, MB_OK | MB_ICONERROR ); \
		action \
	} \
}

#define PLUGIN_CHECK_ERROR_NO_MB( condition, action ) \
{ \
	if( !( condition ) ) \
	{ \
		action \
	} \
}
