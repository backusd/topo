#include <Topo.h>


class Sandbox : public topo::Application
{
public:
	Sandbox()
	{
		TOPO_CORE_INFO("First message");
		TOPO_CORE_INFO("Second Message {}", 42);		
		TOPO_CORE_WARN("This is a warning: {}", "you suck");
		TOPO_CORE_TRACE("Tracing: {}", __LINE__);
		TOPO_CORE_ERROR("ERROR: {}", 1234);

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

topo::Application* topo::CreateApplication()
{
	return new Sandbox();
}