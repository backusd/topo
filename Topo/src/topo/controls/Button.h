#pragma once
#include "topo/Core.h"
#include "Control.h"


namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API Button : public Control
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
#pragma warning ( pop )
}