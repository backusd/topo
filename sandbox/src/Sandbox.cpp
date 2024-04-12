#include <Topo.h>
#include <topo/EntryPoint.h>

#include "MainPage.h"

class Sandbox : public topo::Application
{
public:
	Sandbox() : topo::Application(topo::WindowProperties("Main Window", 1600, 800))
	{
		InitializeMainWindowPage<MainPage>();

//		topo::WindowProperties props{};
//		props.Title = "Child Window";
//		props.Height = 600;
//		props.Width = 600;
//		LaunchWindow<MainPage>(props);

		auto path = std::filesystem::current_path();
		LOG_INFO("PATH: {0}", path.string());
	}
	~Sandbox()
	{
	}

};

std::unique_ptr<topo::Application> topo::CreateApplication()
{
	return std::make_unique<Sandbox>();
}