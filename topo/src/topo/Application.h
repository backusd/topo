#pragma once
#include "Core.h"
#include "events/MouseButtonEventKeyStates.h"
#include "Window.h"
#include "KeyCode.h"
#include "utils/Timer.h"
#include "utils/Concepts.h"

namespace topo
{
class Application
{
public:
	Application(const WindowProperties& mainWindowProperties) noexcept;
	virtual ~Application() noexcept {}
	int Run();
		
	ND constexpr bool ApplicationShutdownRequested() const noexcept { return m_applicationShutdownRequested; }

	template<typename T> requires std::derived_from<T, ::topo::Page>
	bool LaunchWindow(const WindowProperties& props) noexcept;

	static Application* Get() noexcept { return s_application; }

protected:
	template<typename T> requires std::derived_from<T, ::topo::Page>
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

template<typename T> requires std::derived_from<T, ::topo::Page>
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
					window.PrepareToRun();

					Timer timer{};
					timer.Reset();

					while (true)
					{
						timer.Tick();

						// We MUST call window->Update() BEFORE attempting to process messages
						// This is because window->Update() will reset the command list so that new commands may be issued.
						// This is important because processing input may invoke the need to use the command list, but it
						// would be closed. In this scenario, we would have to queue up those commands for when then command
						// list is reset. Instead, it is just easier/safer to make sure the command list is reset before
						// processing input
						window.Update(m_timer);

						if (this->ApplicationShutdownRequested())
							break;

						// process all messages pending, but do not block for new messages
						if (const auto ecode = window.ProcessMessages())
						{
							// if return optional has value, means we're quitting
							return;
						}

						window.Render(m_timer);
						window.Present();
					}
				}
#ifndef TOPO_DIST
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
#else
				// We don't do logging in dist builds, so just catch any exception and terminate
				catch (...)
				{
					return;
				}
#endif
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

std::unique_ptr<Application> CreateApplication();
}