#include "stdafx.h"
#include "OllyExtAssembler.h"


std::hash_map<std::string, DWORD> g_asmLabels;
DWORD g_asmCodeBase = NULL;
BYTE* g_asmBuffer = NULL;
