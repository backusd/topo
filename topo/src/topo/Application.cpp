#include "pch.h"
#include "Application.h"

#include "rendering/ConstantBuffer.h"
#include "utils/GeometryGenerator.h"

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

	ConstantBufferStatic<float> buffer(m_window.GetDeviceResources(), 1);	
	SET_DEBUG_NAME(buffer, "Test Name")



	GeometryGenerator::MeshData sphere = GeometryGenerator::CreateSphere(1.0f, 10, 10);
}
Application::~Application() noexcept
{
}

int Application::Run()
{
	if (!m_window.Initialized())
	{
		LOG_ERROR("Cannot call Application::Run() before initializing the main window");
		return 1;
	}

	m_timer.Reset();

	while (true)
	{
		// process all messages pending, but do not block for new messages
		if (const auto ecode = m_window.ProcessMessages())
		{
			// When the main window is closed, call Terminate to close all open windows
			TerminateAllChildWindows();

			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		try
		{
			m_timer.Tick();
			m_window.DoFrame(m_timer);
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