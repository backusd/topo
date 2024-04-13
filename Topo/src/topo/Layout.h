#pragma once
#include "Core.h"
#include "controls/Control.h"

namespace topo
{
class Layout
{
public:
	Layout(float width, float height) : m_height(height), m_width(width) 
	{}
	Layout(Layout&&) = default;
	Layout& operator=(Layout&&) = default;

	template<typename T>
	void AddControl()
	{
		m_controls.push_back(std::make_unique<T>());
	}

private:
	Layout(const Layout&) = delete;
	Layout& operator=(const Layout&) = delete;

	float m_height;
	float m_width;

	std::vector<std::unique_ptr<Control>> m_controls;
};

}