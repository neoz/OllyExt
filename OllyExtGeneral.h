#pragma once


typedef enum
{
	CODE_MASMSYNTAX,
	CODE_GOASMSYNTAX,
	CODE_NASMSYNTAX,
	CODE_ATSYNTAX
} eCodeRipperSyntax;

typedef enum
{
	DATA_CSYNTAX,
	DATA_MASMSYNTAX
} eDataRipperSyntax;

typedef struct
{
	eCodeRipperSyntax codeRipperSyntax;
	eDataRipperSyntax dataRipperSyntax;
} sGeneralOptions;


extern sGeneralOptions g_generalOptions;


void generalReadOptions( void );
void generalWriteOptions( void );
