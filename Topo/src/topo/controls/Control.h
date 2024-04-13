#pragma once
#include "topo/Core.h"



namespace topo
{
class Control
{
public:
	Control() {}
	Control(const Control&) {}
	Control(Control&&) noexcept {}
	Control& operator=(const Control&) { return *this; }
	Control& operator=(Control&&) noexcept { return *this; }

private:
	int i = 0;
};
}