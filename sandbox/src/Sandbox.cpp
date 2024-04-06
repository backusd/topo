#include <Topo.h>
#include "MainPage.h"

class Sandbox : public topo::Application
{
public:
	Sandbox()
	{
		LOG_INFO("First message");
		LOG_INFO("Second Message {}", 42);
		LOG_WARN("This is a warning: {}", "you suck");
		LOG_TRACE("Tracing: {}", __LINE__);
		LOG_ERROR("ERROR: {}", 1234);

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