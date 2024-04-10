#pragma once
#include <Topo.h>

class MainPage : public topo::Page 
{
public:
	MainPage(float height, float width) :
		topo::Page(height, width)
	{
		m_layout.AddControl<topo::Button>();
	}


};