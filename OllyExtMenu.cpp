#include "stdafx.h"
#include "OptionsDialog.h"
#include "OllyExtAbout.h"
#include "OllyExtMenu.h"


t_menu g_mainMenu[] =
{
	{ L"Options", L"Open Options window", K_NONE, menuOpenOptions, NULL, 0 },
	{ L"|About", L"About Bookmarks plugin", K_NONE, menuAbout, NULL, 0 },
	{ NULL, NULL, K_NONE, NULL, NULL, 0 }
};


int menuOpenOptions( t_table* pt, wchar_t* name, ulong index, int mode )
{
	if( mode == MENU_VERIFY )
	{
		return MENU_NORMAL;
	}
	else if( mode == MENU_EXECUTE )
	{
		DialogBoxParam( g_instance, MAKEINTRESOURCE( IDD_OPTIONS ), NULL, OptionsDlgProc, 0 );
		return MENU_NOREDRAW;
	}

	return MENU_ABSENT;
}


int menuAbout( t_table* pt, wchar_t* name, ulong index, int mode )
{
	if( mode == MENU_VERIFY )
	{
		return MENU_NORMAL;
	}
	else if( mode == MENU_EXECUTE )
	{
	    Resumeallthreads();
		aboutShow();
		Suspendallthreads();
		return MENU_NOREDRAW;
	}

	return MENU_ABSENT;
}
