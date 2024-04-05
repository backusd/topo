#include "pch.h"
#include "Log.h"

namespace topo
{
std::string app_current_time_and_date() noexcept
{
	try
	{
		auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
		return std::format("{:%X}", time);
	}
	catch (const std::runtime_error& e)
	{
		return std::format("Caught runtime error: {}", e.what());
	}
}
}