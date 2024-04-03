#include <Topo.h>


class Sandbox : public topo::Application
{

};

topo::Application* topo::CreateApplication()
{
	return new Sandbox();
}