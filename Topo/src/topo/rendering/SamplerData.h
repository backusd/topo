#pragma once


// Windows defines a 'DOMAIN' macro, so we undefine it here so we can use it below
#pragma push_macro("DOMAIN")
#undef DOMAIN

namespace topo
{
    enum class FILTER
    {
        MIN_MAG_MIP_POINT = 0,
        MIN_MAG_POINT_MIP_LINEAR = 0x1,
        MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
        MIN_POINT_MAG_MIP_LINEAR = 0x5,
        MIN_LINEAR_MAG_MIP_POINT = 0x10,
        MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
        MIN_MAG_LINEAR_MIP_POINT = 0x14,
        MIN_MAG_MIP_LINEAR = 0x15,
        ANISOTROPIC = 0x55,
        COMPARISON_MIN_MAG_MIP_POINT = 0x80,
        COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
        COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
        COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
        COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
        COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
        COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
        COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
        COMPARISON_ANISOTROPIC = 0xd5,
        MINIMUM_MIN_MAG_MIP_POINT = 0x100,
        MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
        MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
        MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
        MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
        MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
        MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
        MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
        MINIMUM_ANISOTROPIC = 0x155,
        MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
        MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
        MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
        MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
        MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
        MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
        MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
        MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
        MAXIMUM_ANISOTROPIC = 0x1d5
    };

    enum class TEXTURE_ADDRESS_MODE
    {
        WRAP = 1,
        MIRROR = 2,
        CLAMP = 3,
        BORDER = 4,
        MIRROR_ONCE = 5
    };

    enum class COMPARISON_FUNC
    {
        NEVER = 1,
        LESS = 2,
        EQUAL = 3,
        LESS_EQUAL = 4,
        GREATER = 5,
        NOT_EQUAL = 6,
        GREATER_EQUAL = 7,
        ALWAYS = 8
    };

    enum class STATIC_BORDER_COLOR
    {
        TRANSPARENT_BLACK = 0,
        OPAQUE_BLACK = (TRANSPARENT_BLACK + 1),
        OPAQUE_WHITE = (OPAQUE_BLACK + 1)
    };

    enum class SHADER_VISIBILITY
    {
        ALL = 0,
        VERTEX = 1,
        HULL = 2,
        DOMAIN = 3,
        GEOMETRY = 4,
        PIXEL = 5,
        AMPLIFICATION = 6,
        MESH = 7
    };

	struct SamplerData
	{
        unsigned int            ShaderRegister = 0;
        FILTER                  Filter = FILTER::ANISOTROPIC;
        TEXTURE_ADDRESS_MODE    AddressU = TEXTURE_ADDRESS_MODE::WRAP;
        TEXTURE_ADDRESS_MODE    AddressV = TEXTURE_ADDRESS_MODE::WRAP;
        TEXTURE_ADDRESS_MODE    AddressW = TEXTURE_ADDRESS_MODE::WRAP;
        float                   MipLODBias = 0.0f;
        unsigned int            MaxAnisotropy = 16;
        COMPARISON_FUNC         ComparisonFunc = COMPARISON_FUNC::LESS_EQUAL;
        STATIC_BORDER_COLOR     BorderColor = STATIC_BORDER_COLOR::OPAQUE_WHITE;
        float                   MinLOD = 0.0f;
        float                   MaxLOD = FLT_MAX;
        SHADER_VISIBILITY       ShaderVisibility = SHADER_VISIBILITY::ALL;
        unsigned int            RegisterSpace = 0;
	};
}

#pragma pop_macro("DOMAIN")