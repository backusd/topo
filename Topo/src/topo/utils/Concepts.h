#pragma once

template <typename T>
concept HasFormatterSpecialization = requires(T t)
{
    std::formatter<T>();
};

#ifdef TOPO_PLATFORM_WINDOWS

template<typename T>
concept HasMemberFunctionPositionThatReturnsXMFLOAT3 = requires(T t)
{
	{ t.Position() } -> std::convertible_to<DirectX::XMFLOAT3>;
};

#endif