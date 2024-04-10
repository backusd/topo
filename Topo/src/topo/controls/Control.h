#pragma once
#include "topo/Core.h"



namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API Control
{
public:
	Control() {}
	Control(const Control&) {}
	Control(Control&&) {}
	Control& operator=(const Control&) { return *this; }
	Control& operator=(Control&&) { return *this; }

private:
	int i;
};
#pragma warning ( pop )
}