#pragma once
#include "topo/Core.h"
#include "Control.h"


namespace topo
{
class Button : public Control
{
public:
	Button();
	Button(const Button&) {}
	Button(Button&&) noexcept {}
	Button& operator=(const Button&) { return *this; }
	Button& operator=(Button&&) noexcept { return *this; }

private:
	void OneTimeInitialization();

	float m_top = 10.0f;
	float m_left = 50.0f;
	float m_width = 100.0f;
	float m_height = 200.0f;

private:
	static bool m_isInitialized;
};

}