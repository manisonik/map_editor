// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define OEMRESOURCE
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <iostream>
#include <tchar.h>
#include <variant>
#include <map>
#include <iterator>
#include <iterator>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>

// Opengl
#include <gl\gl.h>                                // Header File For The OpenGL32 Library
#include <gl\glu.h>                               // Header File For The GLu32 Library
//#include <gl\glaux.h>                             // Header File For The GLaux Library

#define MSKM_REBAR_ADD WM_APP+1

// Property Box
#define PB_BOOLEAN 0
#define PB_FLOAT 1
#define PB_INTEGER 2
#define PB_STRING 3
#define PB_DIRECTORY 4
#define PB_LIST 8

// Property View
#define PV_BOOLEAN 0
#define PV_FLOAT 1
#define PV_INTEGER 2
#define PV_STRING 3
#define PV_DIRECTORY 4
#define PV_LIST 8

// Property Box Notifications
#define PBN_SELECTED 1

struct _HPVITEM {
	int unused;
};
typedef struct _HPVITEM* HPVITEM;