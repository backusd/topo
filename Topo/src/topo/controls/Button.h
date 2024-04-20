#pragma once
#include "topo/Core.h"
#include "Control.h"


namespace topo
{
class Button : public Control
{
public:
	Button(float left, float top, float right, float bottom);
	Button(const Button&) {}
	Button(Button&&) noexcept {}
	Button& operator=(const Button&) { return *this; }
	Button& operator=(Button&&) noexcept { return *this; }

	ND virtual float GetAutoHeight() const noexcept override { return 20.0f; }
	ND virtual float GetAutoWidth() const noexcept override { return 20.0f; }

private:
	void OneTimeInitialization();

private:
	static bool m_isInitialized;
};

}