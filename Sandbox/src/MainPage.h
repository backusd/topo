#pragma once
#include <Topo.h>



//class MainPageT : public topo::Page
//{
//public:
//	MainPageT(float height, float width) : 
//		topo::Page(height, width)
//	{
//		m_layout.AddControl<topo::Button>();
//	}
//};



class MainPage : public topo::Page 
{
public:
	MainPage(float width, float height);


protected:
	topo::Button* button = nullptr;
};