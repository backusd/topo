#include "pch.h"
#include "Application.h"

namespace topo
{

Application::Application() noexcept :
	m_applicationShutdownRequested(false),
	m_window(nullptr),
	m_timer()
{
}
Application::~Application() noexcept
{
}

int Application::Run()
{
	if (m_window == nullptr)
	{
		LOG_ERROR("Cannot call Application::Run() before launching the main window");
		return 1;
	}

	m_timer.Reset();

	while (true)
	{
		// process all messages pending, but do not block for new messages
		if (const auto ecode = m_window->ProcessMessages())
		{
			// When the main window is closed, call Terminate to close all open windows
			TerminateAllChildWindows();

			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		try
		{
			m_timer.Tick();
			m_window->Update(m_timer); 
			m_window->Render(m_timer); 
			m_window->Present();
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

	// If we reach here, it is because we caught an exception an broke out of the loop. So just 
	// terminate the program.
	TerminateAllChildWindows();
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