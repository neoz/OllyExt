#include "stdafx.h"
#include "OllyExtDataRip.h"
#include "OllyExtDumpMenu.h"


t_menu g_dumpSubMenu[] =
{
	{ L"Data Rip to Clipboard", L"Rip selected data to clipboard", K_NONE, menuDumpRip, NULL, 0 },
	{ NULL, NULL, K_NONE, NULL, NULL, 0 }
};

t_menu g_dumpMenu[] =
{
	{ L"OllyExt", L"OllyExt", K_NONE, NULL, g_dumpSubMenu, 0 },
	{ NULL, NULL, K_NONE, NULL, NULL, 0 }
};


int menuDumpRip( t_table* pt, wchar_t* name, ulong index, int mode )
{
	if( mode == MENU_VERIFY )
	{
		return MENU_NORMAL;
	}
	else if( mode == MENU_EXECUTE )
	{
	    Resumeallthreads();
		dataRipExecute();
		Suspendallthreads();
		return MENU_NOREDRAW;
	}

	return MENU_ABSENT;
}
