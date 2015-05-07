#pragma once


typedef struct
{
	bool caption;
	bool killAntiAttach;
} sBugfixesOptions;


extern sBugfixesOptions g_bugfixOptions;


void bugfixesReadOptions( void );
void bugfixesWriteOptions( void );

bool bugfixesApply( void );
