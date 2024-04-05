#pragma once
#include "Core.h"
#include "events/MouseButtonEventKeyStates.h"
#include "Window.h"

namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API Application
{
	friend class Window;

public:
	Application();
	virtual ~Application();
	int Run();
		
	ND constexpr bool ApplicationShutdownRequested() const noexcept { return m_applicationShutdownRequested; }

	template<typename T>
	bool LaunchWindow(const WindowProperties& props);

private:
	template<typename T>
	bool LaunchChildWindow(const WindowProperties& props);

	// Window Event Handlers
	bool OnWindowCreated(Window* window, float height, float width);
	bool OnWindowClosed(Window* window);
	bool OnWindowResized(Window* window, float height, float width);
	bool OnKillFocus(Window* window);
	bool OnDPIChanged(Window* window);

	// Mouse Event Handlers
	bool OnLButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnLButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnLButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnRButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnRButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnRButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX1ButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX1ButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX1ButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX2ButtonDown(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX2ButtonUp(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX2ButtonDoubleClick(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMouseMoved(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMouseEntered(Window* window, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMouseLeave(Window* window);
	bool OnMouseWheel(Window* window, float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMouseHWheel(Window* window, float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);

	// Keyboard Event Handlers
	bool OnChar(Window* window, unsigned int keyCode, unsigned int repeatCount);
	bool OnKeyDown(Window* window, unsigned int keyCode, unsigned int repeatCount);
	bool OnKeyUp(Window* window, unsigned int keyCode, unsigned int repeatCount);
	bool OnSysKeyDown(Window* window, unsigned int keyCode, unsigned int repeatCount);
	bool OnSysKeyUp(Window* window, unsigned int keyCode, unsigned int repeatCount);		

private:
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
bool Application::LaunchChildWindow(const WindowProperties& props)
{
	try
	{
		m_childWindowThreads.emplace_back(
			[this, props]()
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
		);
		return true;
	}
	catch (std::system_error& e)
	{
		// system_error can be thrown from std::thread constructor if it failed to launch the thread
		TOPO_CORE_ERROR("{0}:{1} - Failed to launch thread for window '{2}'. Caught std::system_error. Message: {3}", __FILE__, __LINE__, props.Title, e.what());
		return false;
	}
	catch (std::exception& e)
	{
		TOPO_CORE_ERROR("{0}:{1} - Failed to launch thread for window '{2}'. Caught std::exception. Message: {3}", __FILE__, __LINE__, props.Title, e.what());
		return false;
	}
	catch (...)
	{
		TOPO_CORE_ERROR("{0}:{1} - Failed to launch thread for window '{2}' - UNKNOWN ERROR", __FILE__, __LINE__, props.Title);
		return false;
	}
}
#pragma warning( pop )

std::unique_ptr<Application> CreateApplication();
}