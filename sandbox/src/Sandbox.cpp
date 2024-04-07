#include <Topo.h>
#include "MainPage.h"

class Sandbox : public topo::Application
{
public:
	Sandbox()
	{
		topo::WindowProperties props = {}; 
		props.Title = "Main Window"; 
		props.Height = 720; 
		props.Width = 1280; 
		LaunchWindow<MainPage>(props);

//		props.Title = "Child Window";
//		props.Height = 600;
//		props.Width = 600;
//		LaunchWindow<MainPage>(props);
	}
	~Sandbox()
	{
	}

};

std::unique_ptr<topo::Application> topo::CreateApplication()
{
	return std::make_unique<Sandbox>();
}