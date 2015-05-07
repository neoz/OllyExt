#include "stdafx.h"
#include "OllyExtRip.h"


bool ripSendToClipboard( const std::string& rCode )
{
	PLUGIN_CHECK_ERROR( OpenClipboard( NULL ), return false;, UNABLE_TO_OPEN_CLIPBOARD );

	PLUGIN_CHECK_ERROR( EmptyClipboard(),
		CloseClipboard();
		return false;,
		UNABLE_TO_CLEAR_CLIPBOARD );

	HGLOBAL clipBuffer = GlobalAlloc( GMEM_DDESHARE, rCode.length() + 1 );
	PLUGIN_CHECK_ERROR( clipBuffer,
		CloseClipboard();
		return false;,
		UNABLE_TO_ALLOCATE_GLOBAL_MEMORY );

	char* buffer = (char*)GlobalLock( clipBuffer );
	PLUGIN_CHECK_ERROR( buffer,
		GlobalFree( clipBuffer );
		CloseClipboard();
		return false;,
		UNABLE_TO_LOCK_GLOBAL_DATA );
	strcpy_s( buffer, rCode.length() + 1, rCode.c_str() );
	PLUGIN_CHECK_ERROR( GlobalUnlock( clipBuffer ),
		GlobalFree( clipBuffer );
		CloseClipboard();
		return false;,
		UNABLE_TO_LOCK_GLOBAL_DATA );

	PLUGIN_CHECK_ERROR( SetClipboardData( CF_TEXT, clipBuffer ),
		GlobalFree( clipBuffer );
		CloseClipboard();
		return false;,
		UNABLE_TO_OPEN_CLIPBOARD );

	PLUGIN_CHECK_ERROR( !GlobalFree( clipBuffer ),
		CloseClipboard();
		return false;,
		UNABLE_TO_FREE_GLOBAL_MEMORY );

	PLUGIN_CHECK_ERROR( CloseClipboard(), return false;, UNABLE_TO_CLOSE_CLIPBOARD );

	return true;
}
