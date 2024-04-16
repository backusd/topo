#include "pch.h"
#include "Application.h"
#include "rendering/AssetManager.h"

namespace topo
{
Application* Application::s_application = nullptr;

Application::Application(const WindowProperties& mainWindowProperties) noexcept :
	m_applicationShutdownRequested(false),
	m_window(mainWindowProperties),
	m_timer()
{
	ASSERT(s_application == nullptr, "Not allowed to create a second instance of Application");
	s_application = this;
	AssetManager::Initialize(m_window.GetDeviceResources());
}

int Application::Run()
{
	try
	{
		m_window.PrepareToRun();
		m_timer.Reset();

		while (true)
		{
			m_timer.Tick();

			// We MUST call window->Update() BEFORE attempting to process messages
			// This is because window->Update() will reset the command list so that new commands may be issued.
			// This is important because processing input may invoke the need to use the command list, but it
			// would be closed. In this scenario, we would have to queue up those commands for when then command
			// list is reset. Instead, it is just easier/safer to make sure the command list is reset before
			// processing input
			m_window.Update(m_timer);

			// process all messages pending, but do not block for new messages
			if (const auto ecode = m_window.ProcessMessages())
			{
				// When the main window is closed, call Terminate to close all open windows
				TerminateAllChildWindows();

				// Need to manually release all assets held by AssetManager, otherwise, there will
				// be dangling resources on shutdown
				AssetManager::Shutdown();

				// if return optional has value, means we're quitting so return exit code
				return *ecode;
			}

			m_window.Render(m_timer);
			m_window.Present();
		}
	}
#ifndef TOPO_DIST
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

	// If we reach here, it is because we caught an exception an broke out of the loop. So just 
	// terminate the program.
	TerminateAllChildWindows();

#else
	// We don't do logging in dist builds, so just catch any exception and terminate
	catch (...)
	{
		TerminateAllChildWindows();
	}
#endif

	return 1;
}
void Application::TerminateAllChildWindows() noexcept
{
	// When termination is requested, set this flag to true so that all child windows will know to exit
	m_applicationShutdownRequested = true;

	// Join all child threads before exiting
	for (auto& thread : m_childWindowThreads)
		thread.join();
}

}