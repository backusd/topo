#pragma once
#include "Core.h"
#include "events/MouseButtonEventKeyStates.h"
#include "Window.h"

namespace topo
{
	class Application
	{
		friend class Window;

	public:
		TOPO_API Application();
		TOPO_API virtual ~Application();
		TOPO_API int Run();

	private:
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
	};

	std::unique_ptr<Application> CreateApplication();
}