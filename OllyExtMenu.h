#pragma once


#include "plugin.h"


extern t_menu g_mainMenu[];


int menuOpenOptions( t_table* pt, wchar_t* name, ulong index, int mode );
int menuAbout( t_table* pt, wchar_t* name, ulong index, int mode );
