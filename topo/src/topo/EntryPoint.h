#pragma once

#ifdef TOPO_PLATFORM_WINDOWS

extern std::unique_ptr<topo::Application> topo::CreateApplication();

int main(int argc, char** argv)
{
	auto app = topo::CreateApplication();
	return app->Run();
}

#endif