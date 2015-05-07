#pragma once


#include "plugin.h"


extern t_menu g_disasmMenu[];


int menuCodeRip( t_table* pt, wchar_t* name, ulong index, int mode );
int menuCodeRipRecursive( t_table* pt, wchar_t* name, ulong index, int mode );
