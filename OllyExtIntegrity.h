#pragma once


void integrityInit( void );
void integrityDestroy( void );
void integritySnapshot( wchar_t* libName );
void integrityRestore( HANDLE hProcess, wchar_t* libName );
