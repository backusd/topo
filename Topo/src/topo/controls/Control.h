#pragma once
#include "topo/Core.h"
#include "topo/utils/Rect.h"


namespace topo
{
class Control
{
public:
	// The default constructor should probably not be used in most cases. Rather, it is there in case we want to create a derived Control
	// type that does not use m_positionRect for some reason
	constexpr Control() noexcept : m_positionRect{} {}
	constexpr Control(float left, float top, float right, float bottom) noexcept : m_positionRect{ left, top, right, bottom } {}
	Control(const Control&) {}
	Control(Control&&) noexcept {}
	Control& operator=(const Control&) { return *this; }
	Control& operator=(Control&&) noexcept { return *this; }

	ND constexpr void SetPositionRect(float left, float top, float right, float bottom) noexcept { m_positionRect = { left, top, right, bottom }; }

	ND virtual float GetAutoHeight() const noexcept { return 0.0f; }
	ND virtual float GetAutoWidth() const noexcept { return 0.0f; }

protected:
	Rect m_positionRect;


// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept { m_name = name; }
	ND const std::string& GetDebugName() const noexcept { return m_name; }
private:
	std::string m_name = "Unnamed Layout";
#endif
};
}