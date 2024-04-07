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
		LOG_ERROR("Caught TopoException");
		LOG_ERROR("\tWHAT: {0}", e.what());
		if (e.hasData())
			LOG_ERROR("\tDATA: {0}", e.dataAsString());
		auto& location = e.where();
		LOG_ERROR("\tWHERE: {0}({1}:{2}), `function` {3}", location.file_name(), location.line(), location.column(), location.function_name());
		LOG_ERROR("STACK TRACE:");
		for (auto iter = e.stack().begin(); iter != (e.stack().end() - 3); ++iter) 
			LOG_ERROR("\t{0}({1}) : {2}", iter->source_file(), iter->source_line(), iter->description());
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