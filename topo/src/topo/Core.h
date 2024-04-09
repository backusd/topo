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

#ifdef TOPO_ENABLE_ASSERTS
#define ASSERT(x, ...) { if (!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#ifdef TOPO_DIST
#define SET_DEBUG_NAME(obj, name)
#define SET_DEBUG_NAME_PTR(obj, name)
#else
#define SET_DEBUG_NAME(obj, name) obj.SetDebugName(name);
#define SET_DEBUG_NAME_PTR(obj, name) obj->SetDebugName(name);
#endif