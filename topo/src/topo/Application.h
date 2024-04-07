#pragma once
#include "Core.h"
#include "events/MouseButtonEventKeyStates.h"
#include "Window.h"
#include "KeyCode.h"

namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API Application
{
public:
	Application() noexcept;
	virtual ~Application() noexcept;
	int Run();
		
	ND constexpr bool ApplicationShutdownRequested() const noexcept { return m_applicationShutdownRequested; }

	template<typename T>
	bool LaunchWindow(const WindowProperties& props);

private:
	template<typename T>
	bool LaunchChildWindow(const WindowProperties& props) noexcept;	


	std::unique_ptr<Window> m_window;
	std::vector<std::thread> m_childWindowThreads;
	bool m_applicationShutdownRequested;
};

template<typename T>
bool Application::LaunchWindow(const WindowProperties& props)
{
	if (m_window != nullptr)
		return LaunchChildWindow<T>(props);

	m_window = std::make_unique<Window>(this, props);
	m_window->InitializePage<T>();
	return true;
}

template<typename T>
bool Application::LaunchChildWindow(const WindowProperties& props) noexcept
{
	try
	{
		m_childWindowThreads.emplace_back(
			[this, props]() noexcept
			{
				try
				{
					std::unique_ptr<Window> window = std::make_unique<Window>(this, props);
					window->InitializePage<T>();

					while (true)
					{
						if (this->ApplicationShutdownRequested())
							break;

						// process all messages pending, but do not block for new messages
						if (const auto ecode = window->ProcessMessages())
						{
							// if return optional has value, means we're quitting so return exit code
							return;
						}
					}
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
			}
		);
		return true;
	}
	catch (std::system_error& e)
	{
		// system_error can be thrown from std::thread constructor if it failed to launch the thread
		LOG_ERROR("{0}:{1} - Failed to launch thread for window '{2}'. Caught std::system_error. Message: {3}", __FILE__, __LINE__, props.Title, e.what());
		return false;
	}
	catch (std::exception& e)
	{
		LOG_ERROR("{0}:{1} - Failed to launch thread for window '{2}'. Caught std::exception. Message: {3}", __FILE__, __LINE__, props.Title, e.what());
		return false;
	}
	catch (...)
	{
		LOG_ERROR("{0}:{1} - Failed to launch thread for window '{2}' - UNKNOWN ERROR", __FILE__, __LINE__, props.Title);
		return false;
	}
}
#pragma warning( pop )

std::unique_ptr<Application> CreateApplication();
}