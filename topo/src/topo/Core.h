#pragma once


#ifdef TOPO_PLATFORM_WINDOWS
#ifdef TOPO_BUILD_DLL
#define TOPO_API __declspec(dllexport)
#else
#define TOPO_API __declspec(dllimport)
#endif
#else
#error Only Supporting Windows
#endif