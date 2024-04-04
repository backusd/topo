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

#define ND [[nodiscard]]

#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#ifdef TOPO_DEBUG
#define CORE_ASSERT(x, ...) { if (!(x)) { TOPO_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define ASSERT(x, ...) { if (!(x)) { TOPO_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define CORE_ASSERT(x, ...)
#define ASSERT(x, ...)
#endif