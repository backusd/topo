#pragma once
#include "Core.h"
#include "events/MouseButtonEventKeyStates.h"
#include "Window.h"
#include "KeyCode.h"
#include "utils/Timer.h"

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

	void TerminateAllChildWindows() noexcept;

	Timer m_timer;
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
				std::unique_ptr<Window> window = nullptr;
				try
				{
					window = std::make_unique<Window>(this, props);
					window->InitializePage<T>();
				}
				catch (const topo::TopoException& e)
				{
					LOG_ERROR("{0}", e);
					return;
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Caught std::exception");
					LOG_ERROR("\tWHAT: {0}", e.what());
					return;
				}

				Timer timer{};
				timer.Reset();

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

					try
					{
						timer.Tick();
						window->Update(timer); 
						window->Render(timer); 
						window->Present(); 
					}
					catch (const topo::TopoException& e) 
					{
						LOG_ERROR("{0}", e); 
						break;
					}
					catch (const std::exception& e) 
					{
						LOG_ERROR("Caught std::exception"); 
						LOG_ERROR("\tWHAT: {0}", e.what()); 
						break;
					}
					catch (...)
					{
						LOG_ERROR("Caught unknown exception"); 
						break;
					}
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
}
#pragma warning( pop )

std::unique_ptr<Application> CreateApplication();
}