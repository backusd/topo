#pragma once
#include "Core.h"
#include "events/MouseButtonEventKeyStates.h"
#include "Window.h"
#include "KeyCode.h"
#include "utils/Timer.h"
#include "utils/Concepts.h"

namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API Application
{
public:
	Application(const WindowProperties& mainWindowProperties) noexcept;
	virtual ~Application() noexcept;
	int Run();
		
	ND constexpr bool ApplicationShutdownRequested() const noexcept { return m_applicationShutdownRequested; }

	template<DerivedFromPage T>
	bool LaunchWindow(const WindowProperties& props) noexcept;

	static Application* Get() noexcept { return s_application; }

protected:
	template<DerivedFromPage T>
	void InitializeMainWindowPage()
	{
		m_window.InitializePage<T>();
	}

private:
	void TerminateAllChildWindows() noexcept;

	Timer m_timer;
	Window m_window;
	std::vector<std::thread> m_childWindowThreads;
	bool m_applicationShutdownRequested;

	// Pointer to singleton
	static Application* s_application;
};

template<DerivedFromPage T>
bool Application::LaunchWindow(const WindowProperties& props) noexcept
{
	try
	{
		m_childWindowThreads.emplace_back(
			[this, props]() noexcept
			{
				try
				{
					Window window(props);
					window.InitializePage<T>();

					Timer timer{};
					timer.Reset();

					while (true)
					{
						if (this->ApplicationShutdownRequested())
							break;

						// process all messages pending, but do not block for new messages
						if (const auto ecode = window.ProcessMessages())
						{
							// if return optional has value, means we're quitting
							return;
						}

						timer.Tick(); 
						window.DoFrame(timer);
					}

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
				catch (...)
				{
					LOG_ERROR("Caught unknown exception");
					return;
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