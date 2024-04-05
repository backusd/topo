#include <Topo.h>


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
	}
	~Sandbox()
	{
	}
};

std::unique_ptr<topo::Application> topo::CreateApplication()
{
	return std::make_unique<Sandbox>();
}