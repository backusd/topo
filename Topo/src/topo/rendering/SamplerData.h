#pragma once
#include "Utility.h"


namespace topo
{
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