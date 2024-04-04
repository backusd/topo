#pragma once


#include <algorithm> 
#include <array>
#include <chrono>
#include <concepts>
#include <deque>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iterator>
#include <iostream>		// <-- Can probably remove this for distribution builds
#include <memory>
#include <optional>
#include <queue>
#include <span>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>





#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
// #define NONLS			// --> Causes build to fail - undeclared identifier
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define NOMINMAX

#define STRICT

#include <Windows.h>


#include <dxgidebug.h> // For DxgiInfoManager
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

// #include "utils/Constants.h"
// #include "utils/d3dx12.h"
// #include "utils/DDSTextureLoader.h"
// #include "utils/MathHelper.h"

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")