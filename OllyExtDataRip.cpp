#include "stdafx.h"
#include "OllyExtVersion.h"
#include "OllyExtRip.h"
#include "OllyExtDataRip.h"
#include "OllyExtGeneral.h"


static bool dataRipGetDataFromRange( std::string& rCode )
{
	rCode = "";

	PLUGIN_CHECK_ERROR( g_ollydbgVersion == 201, return false;, UNABLE_TO_RIP_DATA );

	t_dump* pDataDump = ( t_dump* )( (DWORD)hollyinst + 0x001E6F90 );
//	t_dump* pDataDump = Getcpudisasmdump();
	PLUGIN_CHECK_ERROR( pDataDump, return false;, UNABLE_TO_GET_DISASSEMBLER_DUMP );

	DWORD bufferSize = pDataDump->sel1 - pDataDump->sel0;

	PLUGIN_CHECK_ERROR_NO_MB( pDataDump->sel0 && bufferSize, return false; );

	// Allocate the necessary memory
	BYTE* pBuffer = new BYTE[bufferSize];
	PLUGIN_CHECK_ERROR( pBuffer, return false;, UNABLE_TO_ALLOCATE_LOCAL_MEMORY );

	// Read the assembly from the target binary
	PLUGIN_CHECK_ERROR( Readmemory( pBuffer, pDataDump->sel0, bufferSize, MM_SILENT ),
		delete [] pBuffer;
		return false;,
		UNABLE_TO_READ_REMOTE_PROCESS_MEMORY, pDataDump->sel0, bufferSize );

	switch( g_generalOptions.dataRipperSyntax )
	{
	default:
	case DATA_CSYNTAX:
		rCode += "static unsigned char g_rippedData[] =\r\n{\r\n";
	break;

	case DATA_MASMSYNTAX:
		rCode += "g_rippedData\\\r\n";
	break;
	}

	char numStr[16] = { 0 };
	for( DWORD i = 0; i < bufferSize; ++i )
	{
		if( ( i % 8 ) == 0 )
		{
			rCode += "\t";
		}

		switch( g_generalOptions.dataRipperSyntax )
		{
		default:
		case DATA_CSYNTAX:
			sprintf_s( numStr, sizeof( numStr ), "0x%02X", pBuffer[i] );
			rCode += numStr;

			if( i != bufferSize - 1 )
			{
				rCode += ", ";
			}
		break;

		case DATA_MASMSYNTAX:
			if( ( i % 8 ) == 0 )
			{
				rCode += "db ";
			}

			sprintf_s( numStr, sizeof( numStr ), "0%02Xh", pBuffer[i] );
			rCode += numStr;

			if( ( i % 8 ) != 7 &&
				i != bufferSize - 1 )
			{
				rCode += ", ";
			}
		break;
		}

		if( ( i % 8 ) == 7 )
		{
			rCode += "\r\n";
		}
	}

	switch( g_generalOptions.dataRipperSyntax )
	{
	default:
	case DATA_CSYNTAX:
		rCode += "\r\n};\r\n";
	break;

	case DATA_MASMSYNTAX:
		rCode += "\r\n";
	break;
	}

	// Remove buffer
	delete [] pBuffer;

	return true;
}


void dataRipExecute( void )
{
	std::string dataStr = "";

	PLUGIN_CHECK_ERROR_NO_MB( dataRipGetDataFromRange( dataStr ), return; );
	PLUGIN_CHECK_ERROR_NO_MB( ripSendToClipboard( dataStr ), return; );

	Addtolist( 0, BLACK, L"OllyExt: Data ripped successfully" );
}
