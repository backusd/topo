#pragma once
#include "topo/Page.h"

template <typename T>
concept HasFormatterSpecialization = requires(T t)
{
    std::formatter<T>();
};

template <typename T>
concept DerivedFromPage = std::is_base_of<::topo::Page, T>::value;


#ifdef TOPO_PLATFORM_WINDOWS

template<typename T>
concept HasMemberFunctionPositionThatReturnsXMFLOAT3 = requires(T t)
{
	{ t.Position() } -> std::convertible_to<DirectX::XMFLOAT3>;
};

#endif