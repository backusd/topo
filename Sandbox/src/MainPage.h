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
	MainPage(const std::shared_ptr<topo::UIRenderer>& renderer, float width, float height);


protected:
	topo::Button* button = nullptr;
	topo::Button* button2 = nullptr;
};