#pragma once


#include <windows.h>
#include <shlwapi.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <commctrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stddef.h>
#include <set>
#include <hash_map>
#include <string>
#include <fstream>
#include <sstream>
#include "detours.h"
#include "plugin.h"
#include "beaengine\BeaEngine.h"

#include "resource.h"
#include "OllyExt.h"
#include "OllyExtOS.h"
#include "OllyExtError.h"

using namespace std;
