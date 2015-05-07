#include "stdafx.h"
#include "OllyExtGeneral.h"


sGeneralOptions g_generalOptions = { CODE_MASMSYNTAX };


#define CODERIPPERSYNTAX L"CodeRipperSyntax"
#define DATARIPPERSYNTAX L"DataRipperSyntax"


void generalReadOptions( void )
{
	DWORD data = 0;

	Getfromini( NULL, PLUGIN_NAME, CODERIPPERSYNTAX, L"%d", &data );
	g_generalOptions.codeRipperSyntax = (eCodeRipperSyntax)data;
	if( g_generalOptions.codeRipperSyntax < CODE_MASMSYNTAX )
		g_generalOptions.codeRipperSyntax = CODE_MASMSYNTAX;
	if( g_generalOptions.codeRipperSyntax > CODE_ATSYNTAX )
		g_generalOptions.codeRipperSyntax = CODE_MASMSYNTAX;

	Getfromini( NULL, PLUGIN_NAME, DATARIPPERSYNTAX, L"%d", &data );
	g_generalOptions.dataRipperSyntax = (eDataRipperSyntax)data;
	if( g_generalOptions.dataRipperSyntax < DATA_CSYNTAX )
		g_generalOptions.dataRipperSyntax = DATA_CSYNTAX;
	if( g_generalOptions.dataRipperSyntax > DATA_MASMSYNTAX )
		g_generalOptions.dataRipperSyntax = DATA_CSYNTAX;
}


void generalWriteOptions( void )
{
	Writetoini( NULL, PLUGIN_NAME, CODERIPPERSYNTAX, L"%d", g_generalOptions.codeRipperSyntax );
	Writetoini( NULL, PLUGIN_NAME, DATARIPPERSYNTAX, L"%d", g_generalOptions.dataRipperSyntax );
}
