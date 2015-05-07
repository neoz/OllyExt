#include "stdafx.h"
#include "OllyExtOS.h"


static SYSTEM_INFO g_sysInfo = { 0 };
static eOSType g_osType = OS_UNKNOWN;
static eOSArch g_osArch = ARCH_UNKNOWN;


void osInit( void )
{
	DWORD dwVersion = 0;
	DWORD dwMajorVersion = 0;
	DWORD dwMinorVersion = 0;
	DWORD dwBuild = 0;

	dwVersion = GetVersion();

	// Get the Windows version.
	dwMajorVersion = (DWORD)( LOBYTE( LOWORD( dwVersion ) ) );
	dwMinorVersion = (DWORD)( HIBYTE( LOWORD( dwVersion ) ) );

	// Get the build number.
	if( dwVersion < 0x80000000 )
		dwBuild = (DWORD)( HIWORD( dwVersion ) );

	Addtolist( 0, BLACK, L"OllyExt: Detected windows version: %d.%d (%d)",
				dwMajorVersion,
				dwMinorVersion,
				dwBuild );

	wchar_t* type = L"unknown";
	switch( dwMajorVersion )
	{
		case 5:
			g_osType = OS_XP;
			type = L"XP";
		break;

		case 6:
			g_osType = OS_WIN7;
			type = L"7";
		break;

		default:
			g_osType = OS_UNKNOWN;
			MessageBox( NULL, INVALID_WIN_VERSION, PLUGIN_NAME, MB_OK | MB_ICONERROR );
		break;
	}
	Addtolist( 0, BLACK, L"OllyExt: This version considered as Windows %s", type );

	// Get system info
	GetNativeSystemInfo( &g_sysInfo );

	wchar_t* arch = L"unknown";
	switch( g_sysInfo.wProcessorArchitecture )
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
		g_osArch = ARCH_I386;
		arch = L"i386";
	break;

	case PROCESSOR_ARCHITECTURE_AMD64:
		g_osArch = ARCH_AMD64;
		arch = L"amd64";
	break;

	default:
		MessageBox( NULL, INVALID_ARCHITECTURE, PLUGIN_NAME, MB_OK | MB_ICONERROR );
	break;
	}

	Addtolist( 0, BLACK, L"OllyExt: Detected architecture: %s", arch );
}


void osDestroy( void )
{
}


eOSType osGetType( void )
{
	return g_osType;
}


eOSArch osGetArch( void )
{
	return g_osArch;
}
