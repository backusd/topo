#pragma once
#include "topo/Core.h"

namespace topo
{
struct Rect
{
	float Left = 0.0f;
	float Top = 0.0f;
	float Right = 0.0f;
	float Bottom = 0.0f;
	ND constexpr float Width() const noexcept { return Right - Left; }
	ND constexpr float Height() const noexcept { return Bottom - Top; }
	constexpr void Width(float width) noexcept { Right = Left + width; }
	constexpr void Height(float height) noexcept { Bottom = Top + height; }
	ND constexpr bool ContainsPoint(float x, float y) const noexcept { return Left <= x && Right >= x && Top <= y && Bottom >= y; }

};
}