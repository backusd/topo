#include <Topo.h>
#include "MainPage.h"

class Sandbox : public topo::Application
{
public:
	Sandbox() : topo::Application(topo::WindowProperties("Main Window", 1600, 800))
	{
		InitializeMainWindowPage<MainPage>();


		topo::Camera c{};

//		topo::WindowProperties props{};
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