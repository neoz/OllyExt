#include "stdafx.h"
#include "OllyExtLocal.h"
#include "OllyExtRemote.h"
#include "OllyExtIntegrity.h"


typedef struct
{
	void* dllTextSection;
	DWORD dllTextSectionSize;
} sDllSnapshot;


std::hash_map<std::string, sDllSnapshot> g_dllSnapshots;


static void integrityDiskImageInfo( const wchar_t* libName, void*& pCodeStart, DWORD& codeSize )
{
    const UINT PATH_SIZE = 2 * MAX_PATH;
    TCHAR szFilename[ PATH_SIZE ] = { 0 };
	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hFileMapping = NULL;
	PVOID pBaseAddress = NULL;

	__try
	{
		// Module filename
		HMODULE hModule = localGetModuleHandle( libName );
        if( !hModule ) __leave;
        DWORD szFilenameLen = GetModuleFileName( (HMODULE)hModule, szFilename, PATH_SIZE );
		PLUGIN_CHECK_ERROR( szFilenameLen, __leave;, UNABLE_TO_GET_MODULE_FILENAME, hModule );

		// Open birany file
        hFile = CreateFile( szFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		PLUGIN_CHECK_ERROR( hFile != INVALID_HANDLE_VALUE, __leave;, UNABLE_TO_OPEN_FILE, szFilename );

		// Map file
		hFileMapping = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
		PLUGIN_CHECK_ERROR( hFileMapping, __leave;, UNABLE_TO_CREATE_FILE_MAPPING, hFile );

		// Mapping
		pBaseAddress = MapViewOfFile( hFileMapping, FILE_MAP_READ, 0, 0, 0 );
		PLUGIN_CHECK_ERROR( pBaseAddress, __leave;, UNABLE_TO_MAP_FILE, hFileMapping );

		// DOS header check
        PIMAGE_DOS_HEADER pDOSHeader = NULL;
        pDOSHeader = static_cast<PIMAGE_DOS_HEADER>( pBaseAddress );
		PLUGIN_CHECK_ERROR( pDOSHeader->e_magic == IMAGE_DOS_SIGNATURE, __leave;, INVALID_DOS_SIGNATURE_AT, pDOSHeader );

		// NT header check
		PIMAGE_NT_HEADERS pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
			(PBYTE)pBaseAddress + pDOSHeader->e_lfanew );
		PLUGIN_CHECK_ERROR( pNTHeader->Signature == IMAGE_NT_SIGNATURE, __leave;, INVALID_NT_SIGNATURE_AT, pNTHeader );
		PLUGIN_CHECK_ERROR( pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC, __leave;, INVALID_OPTIONAL_SIGNATURE_AT, pNTHeader );

        PIMAGE_FILE_HEADER pFileHeader = reinterpret_cast<PIMAGE_FILE_HEADER>(
			(PBYTE)&pNTHeader->FileHeader );
        PIMAGE_OPTIONAL_HEADER pOptionalHeader = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(
			(PBYTE)&pNTHeader->OptionalHeader );

		PIMAGE_SECTION_HEADER pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(
            (PBYTE)&pNTHeader->OptionalHeader +
            pNTHeader->FileHeader.SizeOfOptionalHeader );

        DWORD dwEntryPoint = pNTHeader->OptionalHeader.AddressOfEntryPoint;
        UINT nSectionCount = pNTHeader->FileHeader.NumberOfSections;
		bool found = false;
        for( DWORD i = 0; i < nSectionCount; i++ )
        {
            // When we find a Section such that
            // Section Start <= Entry Point <= Section End,
            // we have found the .TEXT Section
            if( pSectionHeader->VirtualAddress <= dwEntryPoint &&
                dwEntryPoint < pSectionHeader->VirtualAddress +
                               pSectionHeader->Misc.VirtualSize )
            {
				found = true;
				break;
			}

            pSectionHeader++;
        }

		// Some dlls have null in entry pont
		if( !found )
		{
			pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(
				(PBYTE)&pNTHeader->OptionalHeader +
				pNTHeader->FileHeader.SizeOfOptionalHeader );

			for( DWORD i = 0; i < nSectionCount; i++ )
			{
				if( !strcmp( (char*)pSectionHeader->Name, ".text" ) )
				{
					found = true;
					break;
				}

				pSectionHeader++;
			}
		}

		PLUGIN_CHECK_ERROR( found, __leave;, UNABLE_TO_FIND_TEXT_SECTION_IN, szFilename );

        DWORD codeAddress = (DWORD)pBaseAddress + pSectionHeader->PointerToRawData;
		codeSize = pSectionHeader->Misc.VirtualSize;

		pCodeStart = new BYTE[codeSize];
		PLUGIN_CHECK_ERROR( pCodeStart, __leave;, UNABLE_TO_ALLOCATE_LOCAL_MEMORY );
	}
	__finally
	{
		if( pBaseAddress ) UnmapViewOfFile( pBaseAddress );
		if( hFileMapping ) CloseHandle( hFileMapping );
		if( hFile != INVALID_HANDLE_VALUE ) CloseHandle( hFile );
	}
}                         


static void integrityMemImageInfo( HANDLE hProcess, const wchar_t* libName, void*& codeStart, DWORD& codeSize )
{
	PVOID pBaseAddress = NULL;

	__try
	{
		HMODULE hModule = remoteGetModuleHandle( hProcess, libName );
        if( !hModule ) __leave;

		MEMORY_BASIC_INFORMATION memInfo;
		if( VirtualQueryEx( hProcess, (LPCVOID)hModule, &memInfo, sizeof( MEMORY_BASIC_INFORMATION ) ) != sizeof( MEMORY_BASIC_INFORMATION ) ) __leave;

		pBaseAddress = new BYTE[memInfo.RegionSize];
		PLUGIN_CHECK_ERROR( pBaseAddress, __leave;, UNABLE_TO_ALLOCATE_LOCAL_MEMORY );

		if( !remoteRead( hProcess, memInfo.BaseAddress, memInfo.RegionSize, pBaseAddress ) ) __leave;

		// DOS header check
        PIMAGE_DOS_HEADER pDOSHeader = static_cast<PIMAGE_DOS_HEADER>( pBaseAddress );
		PLUGIN_CHECK_ERROR( pDOSHeader->e_magic == IMAGE_DOS_SIGNATURE, __leave;, INVALID_DOS_SIGNATURE_AT, pDOSHeader );

		// NT header check
		PIMAGE_NT_HEADERS pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
			(PBYTE)pBaseAddress + pDOSHeader->e_lfanew );
		PLUGIN_CHECK_ERROR( pNTHeader->Signature == IMAGE_NT_SIGNATURE, __leave;, INVALID_NT_SIGNATURE_AT, pNTHeader );
		PLUGIN_CHECK_ERROR( pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC, __leave;, INVALID_OPTIONAL_SIGNATURE_AT, pNTHeader );

        PIMAGE_FILE_HEADER pFileHeader = reinterpret_cast<PIMAGE_FILE_HEADER>(
			(PBYTE)&pNTHeader->FileHeader );
        PIMAGE_OPTIONAL_HEADER pOptionalHeader = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(
			(PBYTE)&pNTHeader->OptionalHeader );

		PIMAGE_SECTION_HEADER pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(
            (PBYTE)&pNTHeader->OptionalHeader +
            pNTHeader->FileHeader.SizeOfOptionalHeader );

        DWORD dwEntryPoint = pNTHeader->OptionalHeader.AddressOfEntryPoint;
        UINT nSectionCount = pNTHeader->FileHeader.NumberOfSections;
		bool found = false;
        for( DWORD i = 0; i < nSectionCount; i++ )
        {
            // When we find a Section such that
            // Section Start <= Entry Point <= Section End,
            // we have found the .TEXT Section
            if( pSectionHeader->VirtualAddress <= dwEntryPoint &&
                dwEntryPoint < pSectionHeader->VirtualAddress +
                               pSectionHeader->Misc.VirtualSize )
            {
				found = true;
				break;
			}

            pSectionHeader++;
        }

		// Some dlls have null in entry pont
		if( !found )
		{
			pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(
				(PBYTE)&pNTHeader->OptionalHeader +
				pNTHeader->FileHeader.SizeOfOptionalHeader );

			for( DWORD i = 0; i < nSectionCount; i++ )
			{
				if( !strcmp( (char*)pSectionHeader->Name, ".text" ) )
				{
					found = true;
					break;
				}

				pSectionHeader++;
			}
		}

		PLUGIN_CHECK_ERROR( found, __leave;, UNABLE_TO_FIND_TEXT_SECTION_IN, libName );

		codeStart = (PVOID)( ( (PBYTE)hModule ) + (SIZE_T)( (PBYTE)pSectionHeader->VirtualAddress ) );
		codeSize = pSectionHeader->Misc.VirtualSize;
	}
	__finally
	{
		if( pBaseAddress ) delete [] pBaseAddress;
	}
}                         


void integrityInit( void )
{
}


void integrityDestroy( void )
{
	std::hash_map<std::string, sDllSnapshot>::iterator it;

	for( it = g_dllSnapshots.begin(); it != g_dllSnapshots.end(); ++it )
	{
		sDllSnapshot& snapshot = (*it).second;
		if( snapshot.dllTextSection )
			delete [] snapshot.dllTextSection;
	}

	g_dllSnapshots.clear();
}


void integritySnapshot( wchar_t* libName )
{
	char libNameA[TEXTLEN];
	Unicodetoascii( libName, wcslen( libName ), libNameA, TEXTLEN );

	// Get dll .text parameters from memory
	HANDLE hProcess = GetCurrentProcess();
	sDllSnapshot snapshot;
	void* codeStart = NULL;
	integrityMemImageInfo( hProcess, L"ntdll.dll", codeStart, snapshot.dllTextSectionSize );
	if( !codeStart ) return;

	// Load dll .text content from memory
	snapshot.dllTextSection = new BYTE[snapshot.dllTextSectionSize];
	PLUGIN_CHECK_ERROR( snapshot.dllTextSection, return;, UNABLE_TO_ALLOCATE_LOCAL_MEMORY );
	if( !remoteRead( hProcess, codeStart, snapshot.dllTextSectionSize, snapshot.dllTextSection ) ) return;

	g_dllSnapshots[libNameA] = snapshot;
}


void integrityRestore( HANDLE hProcess, wchar_t* libName )
{
	char libNameA[TEXTLEN];
	Unicodetoascii( libName, wcslen( libName ), libNameA, TEXTLEN );

	// Read dll .text from snapshots
	sDllSnapshot& snapshot = g_dllSnapshots[libNameA];

	// Get dll .text parameters from memory
	void* codeStart = NULL;
	DWORD codeSize = 0;
	integrityMemImageInfo( hProcess, L"ntdll.dll", codeStart, codeSize );
	if( !codeStart ) return;

	// Load dll .text content from memory
	void* pCodeStart = new BYTE[codeSize];
	PLUGIN_CHECK_ERROR( pCodeStart,
		delete [] pCodeStart; \
		return;,
		UNABLE_TO_ALLOCATE_LOCAL_MEMORY );
	if( !remoteRead( hProcess, codeStart, codeSize, pCodeStart ) )
	{
		delete [] pCodeStart;
		return;
	}

	// Compare them
	int cmpResult = memcmp( snapshot.dllTextSection, pCodeStart, snapshot.dllTextSectionSize );
	if( cmpResult )
		remoteWrite( hProcess, snapshot.dllTextSection, codeSize, codeStart );

	delete [] pCodeStart;
}
