#include "pch.h"
#include "Application.h"

namespace topo
{

Application::Application() noexcept :
	m_applicationShutdownRequested(false),
	m_window(nullptr)
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

	while (true)
	{
		// process all messages pending, but do not block for new messages
		if (const auto ecode = m_window->ProcessMessages())
		{
			// When the main window is closed, set this flag to true so that all child windows will know to exit
			m_applicationShutdownRequested = true;

			// Join all child threads before exiting
			for (auto& thread : m_childWindowThreads)
				thread.join();

			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
	}
}

}