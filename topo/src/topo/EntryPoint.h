#pragma once

#ifdef TOPO_PLATFORM_WINDOWS

extern std::unique_ptr<topo::Application> topo::CreateApplication();

int main(int argc, char** argv)
{
	try
	{
		auto app = topo::CreateApplication();
		return app->Run();
	}
	catch (const topo::TopoException& e)
	{
		LOG_ERROR("{0}", e);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Caught std::exception");
		LOG_ERROR("\tWHAT: {0}", e.what()); 
	}
	catch (...)
	{
		LOG_ERROR("Caught unknown exception");
	}

	return 1;
}

#endif