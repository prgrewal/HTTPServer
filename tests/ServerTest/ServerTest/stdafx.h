// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN 

// Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string>
#include <vector>
#include <iostream>
// reference additional headers your program requires here
#include <Commctrl.h>
#pragma comment( lib, "comctl32.lib" ) 
#pragma comment( lib, "Ws2_32.lib" ) 
//#pragma comment( lib, "Ws2_32.dll" ) 
