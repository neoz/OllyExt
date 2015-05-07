#pragma once


typedef enum
{
	OS_UNKNOWN,
	OS_XP,
	OS_WIN7
} eOSType;

typedef enum
{
	ARCH_UNKNOWN,
	ARCH_I386,
	ARCH_AMD64
} eOSArch;


void osInit( void );
void osDestroy( void );
eOSType osGetType( void );
eOSArch osGetArch( void );
