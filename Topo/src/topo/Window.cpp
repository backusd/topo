#include "pch.h"
#include "Window.h"
#include "Application.h"

#include <windowsx.h> // Included so we can use GET_X_LPARAM/GET_Y_LPARAM

namespace topo
{
#ifdef TOPO_PLATFORM_WINDOWS
// =======================================================================
// Window
// =======================================================================
Window::Window(Application* app, const WindowProperties& props) :
	WindowTemplate(props),
	m_app(app),
	m_page(nullptr)
{
}
void Window::Shutdown()
{
	UnregisterClass(wndBaseClassName, m_hInst); 
	DestroyWindow(m_hWnd); 
}
std::optional<int> Window::ProcessMessages() const noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return static_cast<int>(msg.wParam);
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);

		// Can optionally obtain the LRESULT value that is returned, but from the Microsoft docs:
		// "The return value specifies the value returned by the window procedure. Although its 
		// meaning depends on the message being dispatched, the return value generally is ignored."
		// LRESULT result = DispatchMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//static seethe::WindowMessageMap mm;
	//LOG_TRACE("{}", mm(msg, wParam, lParam));

	// 'case' options are arranged in roughly in the order of highest frequency
	// I'm not sure if this actually affects performance slightly, but I assume it can't hurt
	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
		m_mouseX = static_cast<float>(GET_X_LPARAM(lParam)); 
		m_mouseY = static_cast<float>(GET_Y_LPARAM(lParam)); 

		if (m_mouseX >= 0 && m_mouseX < m_width && m_mouseY >= 0 && m_mouseY < m_height)
		{
			if (!m_mouseIsInWindow) // Will tell you if the mouse was PREVIOUSLY in the window or not
			{
				m_mouseIsInWindow = true;

				SetCapture(hWnd);

				// NOTE: We only call enter OR move, not both. The mouse enter event should also be treated like a move event
				if (m_app->OnMouseEntered(this, m_mouseX, m_mouseY, MouseButtonEventKeyStates(wParam)))
					return 0;
				break;
			}

			if (m_app->OnMouseMoved(this, m_mouseX, m_mouseY, MouseButtonEventKeyStates(wParam)))
				return 0;
			break;
		}
		else
		{
			m_mouseIsInWindow = false;

			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
			{
				if (m_app->OnMouseMoved(this, m_mouseX, m_mouseY, MouseButtonEventKeyStates(wParam))) 
					return 0;
				break;
			}
		}

		// If we reach here, the mouse is NOT over the window and no mouse buttons are down.
		ReleaseCapture();
		if (m_app->OnMouseLeave(this))
			return 0;
		break;
	}
	case WM_MOUSELEAVE:	 
		if (m_app->OnMouseLeave(this)) 
			return 0;
		break;

	// LButton
	case WM_LBUTTONDOWN: 
		if (m_app->OnLButtonDown(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_LBUTTONUP:
		if (m_app->OnLButtonUp(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_LBUTTONDBLCLK:	
		if (m_app->OnLButtonDoubleClick(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break; 

	// RButton
	case WM_RBUTTONDOWN:	
		if (m_app->OnRButtonDown(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_RBUTTONUP:
		if (m_app->OnRButtonUp(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_RBUTTONDBLCLK:  
		if (m_app->OnRButtonDoubleClick(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;

	// MButton
	case WM_MBUTTONDOWN:
		if (m_app->OnMButtonDown(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_MBUTTONUP:
		if (m_app->OnRButtonUp(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_MBUTTONDBLCLK:  
		if (m_app->OnMButtonDoubleClick(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;

	// X1/X2 Buttons
	case WM_XBUTTONDOWN:
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
		{
			if (m_app->OnX1ButtonDown(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		else
		{
			if (m_app->OnX2ButtonDown(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		break;
	case WM_XBUTTONUP:
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
		{
			if (m_app->OnX1ButtonUp(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		else
		{
			if (m_app->OnX2ButtonUp(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		break;
	case WM_XBUTTONDBLCLK:
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
		{
			if (m_app->OnX1ButtonDoubleClick(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		else
		{
			if (m_app->OnX2ButtonDoubleClick(this, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		break;


	case WM_SIZE:
	{
		m_width = LOWORD(lParam);
		m_height = HIWORD(lParam);
//		m_deviceResources->OnResize(m_height, m_width);

		if (m_app->OnWindowResized(this, m_height, m_width))
			return 0;
		break;
	}

	// Mouse Wheel
	case WM_MOUSEWHEEL:		
		if (m_app->OnMouseWheel(this, static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)), static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
		
	case WM_MOUSEHWHEEL:
		if (m_app->OnMouseHWheel(this, static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)), static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;


	// Keyboard Events
	case WM_CHAR:			
		if (m_app->OnChar(this, static_cast<unsigned int>(wParam), static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;		
	case WM_KEYDOWN:		
		if (m_app->OnKeyDown(this, static_cast<unsigned int>(wParam), static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;
	case WM_KEYUP:			
		if (m_app->OnKeyUp(this, static_cast<unsigned int>(wParam), static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;
	case WM_SYSKEYDOWN:		
		if (m_app->OnSysKeyDown(this, static_cast<unsigned int>(wParam), static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;
	case WM_SYSKEYUP:		
		if (m_app->OnSysKeyUp(this, static_cast<unsigned int>(wParam), static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;


	// Window Events
	case WM_KILLFOCUS:		
		if (m_app->OnKillFocus(this))
			return 0;
		break;
		
	case WM_CREATE:
	{
		CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		m_height = cs->cy;
		m_width = cs->cx;
//		m_deviceResources = std::make_shared<DeviceResources>(hWnd, height, width);

		if (m_app->OnWindowCreated(this, m_height, m_width))
			return 0;
		break;
	}
	case WM_CLOSE:			
		if (m_app->OnWindowClosed(this))
			return 0;
		break;

	case WM_DPICHANGED:
		TOPO_CORE_WARN("({0} - {1}: Received WM_DPICHANGED. Currently not handling this message", __FILE__, __LINE__); 
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#else
#error Only Supporting Windows
#endif
}