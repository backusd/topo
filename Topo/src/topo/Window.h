#pragma once
#include "Core.h"
#include "Log.h"

namespace topo
{
class Application;

#ifdef TOPO_PLATFORM_WINDOWS

// =======================================================================
// Window Template
// =======================================================================
template<typename T>
class WindowTemplate
{
public:
	WindowTemplate(std::string_view title, unsigned int width, unsigned int height) :
		m_height(height), 
		m_width(width),
		m_title(title), 
		m_hInst(GetModuleHandle(nullptr)), // I believe GetModuleHandle should not ever throw, even though it is not marked noexcept
		m_mouseX(0),
		m_mouseY(0),
		m_mouseIsInWindow(false)
	{
		// Register the window class
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC | CS_DBLCLKS;
		wc.lpfnWndProc = HandleMsgSetupBase;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInst;
		wc.hIcon = nullptr;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = wndBaseClassName;
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);

		// calculate window size based on desired client region size
		RECT rect = {};
		rect.left = 100;
		rect.right = m_width + rect.left;
		rect.top = 100;
		rect.bottom = m_height + rect.top;

		auto WS_options = WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_SIZEBOX;

		if (AdjustWindowRect(&rect, WS_options, FALSE) == 0)
		{
			throw std::runtime_error("ERROR LINE 47");
//				throw WINDOW_LAST_EXCEPT();
		};

		// TODO: Look into other extended window styles
		auto style = WS_EX_WINDOWEDGE;

		std::wstring w_title(m_title.begin(), m_title.end());

		// create window & get hWnd
		m_hWnd = CreateWindowExW(
			style,
			wndBaseClassName,
			w_title.c_str(),
			WS_options,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr,
			nullptr,
			m_hInst,
			this
		);

		if (m_hWnd == nullptr)
		{
			throw std::runtime_error("ERROR LINE 74");
//				throw WINDOW_LAST_EXCEPT();
		}
		// show window
		ShowWindow(m_hWnd, SW_SHOWDEFAULT);

		TOPO_CORE_INFO("Created window: {0} ({1}, {2})", m_title, m_height, m_width);
	}
	WindowTemplate(const WindowTemplate&) = delete;
	WindowTemplate(WindowTemplate&&) = delete;
	WindowTemplate& operator=(const WindowTemplate&) = delete;
	WindowTemplate& operator=(WindowTemplate&&) = delete;
	virtual ~WindowTemplate()
	{
		UnregisterClass(wndBaseClassName, m_hInst);
		DestroyWindow(m_hWnd);
	};

	ND constexpr HWND GetHWND() const noexcept { return m_hWnd; }
	ND constexpr short GetWidth() const noexcept { return m_width; }
	ND constexpr short GetHeight() const noexcept { return m_height; }
	ND constexpr short GetMouseX() const noexcept { return m_mouseX; }
	ND constexpr short GetMouseY() const noexcept { return m_mouseY; }
	ND constexpr bool MouseIsInWindow() const noexcept { return m_mouseIsInWindow; }

	inline void BringToForeground() const noexcept { if (m_hWnd != ::GetForegroundWindow()) ::SetForegroundWindow(m_hWnd); }

protected:
	ND static LRESULT CALLBACK HandleMsgSetupBase(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	ND static LRESULT CALLBACK HandleMsgBase(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Window Class Data
	static constexpr const wchar_t* wndBaseClassName = L"Topo Window";
	HINSTANCE m_hInst;

	// Window Data
	short m_width;
	short m_height;
	std::string m_title;
	HWND m_hWnd;
	float m_mouseX;
	float m_mouseY;
	bool m_mouseIsInWindow;
};

template<typename T>
LRESULT CALLBACK WindowTemplate<T>::HandleMsgSetupBase(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
//		static seethe::WindowMessageMap mm;
//		LOG_TRACE("EARLY MESSAGE: {}", mm(msg, lParam, wParam));

	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowTemplate<T>* const pWnd = static_cast<WindowTemplate<T>*>(pCreate->lpCreateParams);

		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowTemplate<T>::HandleMsgBase));
	}

	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

template<typename T>
LRESULT CALLBACK WindowTemplate<T>::HandleMsgBase(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to window class & forward message to window class handler
	T* const pWnd = reinterpret_cast<T*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

// =======================================================================
// Window
// =======================================================================
class Window : public WindowTemplate<Window>
{
public:
	Window(Application* app, std::string_view title = "Topo App", unsigned int width = 1280, unsigned int height = 720);
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;
	virtual ~Window() override { Shutdown(); }		

	ND std::optional<int> ProcessMessages() const noexcept;
	ND LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//		ND inline std::shared_ptr<DeviceResources> GetDeviceResources() noexcept { return m_deviceResources; }


private:
	void Shutdown();
	
//		std::shared_ptr<DeviceResources> m_deviceResources = nullptr;
	Application* m_app;
};

#else
#error Only Supporting Windows
#endif
}