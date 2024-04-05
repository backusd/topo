#include "pch.h"
#include "Application.h"

namespace topo
{

Application::Application() :
	m_applicationShutdownRequested(false),
	m_window(nullptr)
{
}
Application::~Application()
{
}

int Application::Run()
{
	if (m_window == nullptr)
	{
		TOPO_CORE_ERROR("Cannot call Application::Run() before launching the main window");
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

// Window Event Handlers
bool Application::OnWindowCreated(Window* window, float height, float width)
{
	return true;
}
bool Application::OnWindowClosed(Window* window)
{
	PostQuitMessage(0);
	return true;
}
bool Application::OnWindowResized(Window* window, float height, float width)
{
	return true;
}
bool Application::OnKillFocus(Window* window)
{
	return true;
}
bool Application::OnDPIChanged(Window* window)
{
	return true;
}

// Mouse Event Handlers
bool Application::OnLButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnLButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnLButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnMButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnMButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnMButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnRButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnRButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnRButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnX1ButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnX1ButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnX1ButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnX2ButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnX2ButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnX2ButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnMouseMoved(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnMouseEntered(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnMouseLeave(Window* window)
{
	return true;
}
bool Application::OnMouseWheel(Window* window, float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Application::OnMouseHWheel(Window* window, float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}

// Keyboard Event Handlers
bool Application::OnChar(Window* window, unsigned int keyCode, unsigned int repeatCount)
{
	return true;
}
bool Application::OnKeyDown(Window* window, unsigned int keyCode, unsigned int repeatCount)
{
	return true;
}
bool Application::OnKeyUp(Window* window, unsigned int keyCode, unsigned int repeatCount)
{
	return true;
}
bool Application::OnSysKeyDown(Window* window, unsigned int keyCode, unsigned int repeatCount)
{
	return true;
}
bool Application::OnSysKeyUp(Window* window, unsigned int keyCode, unsigned int repeatCount)
{
	return true;
}
}