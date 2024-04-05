//#include <topo/EntryPoint.h>
#include <Topo.h>

#include "MainPage.h"

class Sandbox : public topo::Application
{
public:
	Sandbox()
	{
		TOPO_INFO("First message");
		TOPO_INFO("Second Message {}", 42);
		TOPO_WARN("This is a warning: {}", "you suck");
		TOPO_TRACE("Tracing: {}", __LINE__);
		TOPO_ERROR("ERROR: {}", 1234);

		topo::WindowProperties props = {}; 
		props.Title = "Main Window"; 
		props.Height = 720; 
		props.Width = 1280; 
		LaunchWindow<MainPage>(props);

		props.Title = "Child Window";
		props.Height = 600;
		props.Width = 600;
		LaunchWindow<MainPage>(props);
	}
	~Sandbox()
	{
	}

};

std::unique_ptr<topo::Application> topo::CreateApplication()
{
	return std::make_unique<Sandbox>();
}