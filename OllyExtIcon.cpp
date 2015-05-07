#include "stdafx.h"
#include "OllyExtIcon.h"


HICON g_hIconSmall = NULL;
HICON g_hIconBig = NULL;


void iconInit( void )
{
	g_hIconSmall = LoadIcon( g_instance, MAKEINTRESOURCE( IDI_ICON_SMALL ) );
	g_hIconBig = LoadIcon( g_instance, MAKEINTRESOURCE( IDI_ICON ) );
}


void iconDestroy( void )
{
	if( g_hIconSmall )
	{
		DestroyIcon( g_hIconSmall );
		g_hIconSmall = NULL;
	}
	if( g_hIconBig )
	{
		DestroyIcon( g_hIconBig );
		g_hIconBig = NULL;
	}
}
