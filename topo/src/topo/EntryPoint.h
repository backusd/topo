#pragma once

#ifdef TOPO_PLATFORM_WINDOWS

extern topo::Application* topo::CreateApplication();

int main(int argc, char** argv)
{
	auto app = topo::CreateApplication();
	app->Run();
	delete app;
}

#endif