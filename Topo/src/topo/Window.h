#pragma once
#include "Core.h"
#include "DeviceResources.h"
#include "Log.h"
#include "Page.h"
#include "TopoException.h"
#include "utils/String.h"
#include "utils/TranslateErrorCode.h"
#include "utils/Timer.h"
#include "rendering/Renderer.h"

#include "rendering/Texture.h"

#ifdef TOPO_PLATFORM_WINDOWS
#define THROW_WINDOW_LAST_EXCEPT() auto _err = GetLastError(); throw EXCEPTION(std::format("Window Exception\n[Error Code] {0:#x} ({0})\n[Description] {1}", _err, ::topo::TranslateErrorCode(_err)))
#else
#error Only Supporting Windows!
#endif

namespace topo
{
	struct Light
	{
		DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
		float FalloffStart = 1.0f;                          // point/spot light only
		DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
		float FalloffEnd = 10.0f;                           // point/spot light only
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
		float SpotPower = 64.0f;                            // spot light only
	};

#define MaxLights 16

	struct PassConstants
	{
		float Width;
		float Height;
	};
	struct CratePassConstants
	{
		DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;

		DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Indices [0, NUM_DIR_LIGHTS) are directional lights;
		// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
		// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
		// are spot lights for a maximum of MaxLights per object.
		Light Lights[MaxLights];
	};

	struct Vertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 Position() const noexcept { return { position.x, position.y, position.z }; }
	};
	struct CrateVertex
	{
		DirectX::XMFLOAT4 Color = { 0.75f, 0.0f, 0.0f, 1.0f };

		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexC;
		DirectX::XMFLOAT3 Position() const noexcept { return Pos; }
	};
	struct Material
	{
//		// Unique material name for lookup.
//		std::string Name;
//
//		// Index into constant buffer corresponding to this material.
//		int MatCBIndex = -1;
//
//		// Index into SRV heap for diffuse texture.
//		int DiffuseSrvHeapIndex = -1;
//
//		// Index into SRV heap for normal texture.
//		int NormalSrvHeapIndex = -1;
//
//		// Dirty flag indicating the material has changed and we need to update the constant buffer.
//		// Because we have a material constant buffer for each FrameResource, we have to apply the
//		// update to each FrameResource.  Thus, when we modify a material we should set 
//		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
//		int NumFramesDirty = gNumFrameResources;

		// Material constant buffer data used for shading.
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = .25f;
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
	};
	struct ObjectData
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
	};







struct WindowProperties
{
	constexpr WindowProperties(std::string_view title = "Topo Window", unsigned int width = 1280, unsigned int height = 720) noexcept :
		Title(title), Width(width), Height(height)
	{}

	std::string_view Title = "Topo Window";
	unsigned int Width = 1280;
	unsigned int Height = 720;
};

#ifdef TOPO_PLATFORM_WINDOWS

// =======================================================================
// Window Template
// =======================================================================
template<typename T>
class WindowTemplate
{
public:
	WindowTemplate(const WindowProperties& props) :
		m_deviceResources(nullptr),
		m_renderer(nullptr),
		m_viewport{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		m_scissorRect{ 0, 0, 0, 0 },
		m_page(std::make_unique<Page>(static_cast<float>(props.Width), static_cast<float>(props.Height))),	// Create a default page so it is guaranteed to not be null
		m_height(props.Height), 
		m_width(props.Width),
		m_title(props.Title), 
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
			THROW_WINDOW_LAST_EXCEPT();
		};

		// TODO: Look into other extended window styles
		auto style = WS_EX_WINDOWEDGE;

		std::wstring w_title = s2ws(m_title);

		// create window & get hWnd
		m_hWnd = CreateWindowExW(
			style,
			wndBaseClassName,	// <-- Set this nullptr to get exception to throw
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
			THROW_WINDOW_LAST_EXCEPT();
		}
		// show window
		ShowWindow(m_hWnd, SW_SHOWDEFAULT);

		LOG_INFO("Created window: {0} ({1}, {2})", m_title, m_width, m_height);
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
	ND constexpr float GetMouseX() const noexcept { return m_mouseX; }
	ND constexpr float GetMouseY() const noexcept { return m_mouseY; }
	ND constexpr bool MouseIsInWindow() const noexcept { return m_mouseIsInWindow; }

	inline void BringToForeground() const noexcept { if (m_hWnd != ::GetForegroundWindow()) ::SetForegroundWindow(m_hWnd); }

protected:
	ND static LRESULT CALLBACK HandleMsgSetupBase(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	ND static LRESULT CALLBACK HandleMsgBase(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Window Class Data
	static constexpr const wchar_t* wndBaseClassName = L"Topo Window";
	HINSTANCE m_hInst;

	std::shared_ptr<DeviceResources> m_deviceResources;
	std::unique_ptr<Page> m_page;

	std::unique_ptr<Renderer> m_renderer;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	// 2D Test
	std::unique_ptr<ConstantBufferMapped<PassConstants>>	m_passConstantsBuffer = nullptr;
	std::unique_ptr<MeshGroup<Vertex>> m_meshGroup = nullptr;


	// 3D Test
	std::unique_ptr<MeshGroup<CrateVertex>> m_meshGroupCrate = nullptr;
	std::unique_ptr<ConstantBufferMapped<CratePassConstants>> m_passConstantBufferCrate = nullptr;
	std::unique_ptr<ConstantBufferMapped<Material>> m_materialConstantBuffer = nullptr;
	std::unique_ptr<ConstantBufferMapped<ObjectData>> m_objectConstantBuffer = nullptr;
	DirectX::XMFLOAT3 m_eyePosition = {};
	std::unique_ptr<Camera> m_camera = nullptr;
	std::unique_ptr<Texture> m_texture = nullptr;

	SamplerData m_sd0{};
	SamplerData m_sd1{};
	SamplerData m_sd2{};
	SamplerData m_sd3{};
	SamplerData m_sd4{};
	SamplerData m_sd5{};




	// Window Data
	short		m_width;
	short		m_height;
	std::string	m_title;
	HWND		m_hWnd;
	float		m_mouseX;
	float		m_mouseY;
	bool		m_mouseIsInWindow;
};

template<typename T>
LRESULT CALLBACK WindowTemplate<T>::HandleMsgSetupBase(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
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
	Window(const WindowProperties& props) : WindowTemplate(props) {}
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;
	virtual ~Window() override { Shutdown(); }		

	ND std::optional<int> ProcessMessages() const noexcept;
	ND LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	ND inline std::shared_ptr<DeviceResources> GetDeviceResources() noexcept { return m_deviceResources; }

	template<typename T>
	void InitializePage()
	{
		m_page = std::make_unique<T>(m_height, m_width);
		InitializeRenderer();
	}

	void PrepareToRun();
	void Update(const Timer& timer);
	void Render(const Timer& timer);
	void Present();

private:	
	WPARAM MapLeftRightKeys(WPARAM vk, LPARAM lParam);
	void InitializeRenderer();
	void Shutdown();
};


#else
#error Only Supporting Windows
#endif
}