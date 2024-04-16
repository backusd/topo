#include "pch.h"
#include "Window.h"
#include "Application.h"
#include "KeyCode.h"
#include "Input.h"
#include "utils/WindowMessageMap.h"

#include "rendering/MeshGroup.h"
#include "rendering/AssetManager.h"
#include "utils/GeometryGenerator.h"
#include "rendering/SamplerData.h"
#include "rendering/PipelineStateDesc.h"

#include <windowsx.h> // Included so we can use GET_X_LPARAM/GET_Y_LPARAM

namespace topo
{
#ifdef TOPO_PLATFORM_WINDOWS

// =======================================================================
// Window
// =======================================================================
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
		// check for quit because peekmessage does not signal  via return val
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
//	static topo::WindowMessageMap mm;
//	LOG_TRACE("{}", mm(msg, wParam, lParam));

	// 'case' options are arranged in roughly in the order of highest frequency
	// I'm not sure if  actually affects performance slightly, but I assume it can't hurt
	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
		// NOTE: We don't immediately set the mouse position every time we get WM_MOUSEMOVE
		//       This is because of the infrequent case when a mouse button is down and the mouse
		//		 goes outside the window, then we call SetCapture, but then at some point the button
		//       is released and the mouse is hovering over another window. In this scenario, the other
		//       window actually receives 1 or 2 WM_MOUSEMOVE events because the initial window receives
		//       a WM_MOUSEMOVE event where it will finally call ReleaseCapture. This is necessary
		//       because the Input class is a singleton, so we need to make sure multiple windows aren't
		//       modifying its data

		m_mouseX = static_cast<float>(GET_X_LPARAM(lParam)); 
		m_mouseY = static_cast<float>(GET_Y_LPARAM(lParam)); 

		if (m_mouseX >= 0 && m_mouseX < m_width && m_mouseY >= 0 && m_mouseY < m_height)
		{
			Input::SetMousePosition(m_mouseX, m_mouseY); 

			if (!m_mouseIsInWindow) // Will tell you if the mouse was PREVIOUSLY in the window or not
			{
				m_mouseIsInWindow = true;

				SetCapture(hWnd);

				// NOTE: We only call enter OR move, not both. The mouse enter event should also be treated like a move event
				if (m_page->OnMouseEntered(m_mouseX, m_mouseY, MouseButtonEventKeyStates(wParam)))
					return 0;
				break;
			}

			if (m_page->OnMouseMoved(m_mouseX, m_mouseY, MouseButtonEventKeyStates(wParam)))
				return 0;
			break;
		}
		else
		{
			m_mouseIsInWindow = false;

			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
			{
				Input::SetMousePosition(m_mouseX, m_mouseY);

				if (m_page->OnMouseMoved(m_mouseX, m_mouseY, MouseButtonEventKeyStates(wParam))) 
					return 0;
				break;
			}
		}

		// If we reach here, the mouse is NOT over the window and no mouse buttons are down.
		ReleaseCapture();
		if (m_page->OnMouseLeave())
			return 0;
		break;
	}
	case WM_MOUSELEAVE:	 
		if (m_page->OnMouseLeave()) 
			return 0;
		break;

	// LButton
	case WM_LBUTTONDOWN: 
		BringToForeground();
		Input::SetKeyDownState(KeyCode::LBUTTON, true);
		if (m_page->OnLButtonDown(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_LBUTTONUP:
		Input::SetKeyDownState(KeyCode::LBUTTON, false);
		if (m_page->OnLButtonUp(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_LBUTTONDBLCLK:
		Input::SetKeyDownState(KeyCode::LBUTTON, true); // The double click message is triggered on the second down click and will be followed by another BUTTONUP message
		if (m_page->OnLButtonDoubleClick(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break; 

	// RButton
	case WM_RBUTTONDOWN:
		BringToForeground();
		Input::SetKeyDownState(KeyCode::RBUTTON, true);
		if (m_page->OnRButtonDown(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_RBUTTONUP:
		Input::SetKeyDownState(KeyCode::RBUTTON, false);
		if (m_page->OnRButtonUp(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_RBUTTONDBLCLK:  
		Input::SetKeyDownState(KeyCode::RBUTTON, true); // The double click message is triggered on the second down click and will be followed by another BUTTONUP message
		if (m_page->OnRButtonDoubleClick(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;

	// MButton
	case WM_MBUTTONDOWN:
		BringToForeground();
		Input::SetKeyDownState(KeyCode::MBUTTON, true);
		if (m_page->OnMButtonDown(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_MBUTTONUP:
		Input::SetKeyDownState(KeyCode::MBUTTON, false);
		if (m_page->OnRButtonUp(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
	case WM_MBUTTONDBLCLK:  
		Input::SetKeyDownState(KeyCode::MBUTTON, true); // The double click message is triggered on the second down click and will be followed by another BUTTONUP message
		if (m_page->OnMButtonDoubleClick(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;

	// X1/X2 Buttons
	case WM_XBUTTONDOWN:
		BringToForeground();
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
		{
			Input::SetKeyDownState(KeyCode::X1BUTTON, true);
			if (m_page->OnX1ButtonDown(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		else
		{
			Input::SetKeyDownState(KeyCode::X2BUTTON, true);
			if (m_page->OnX2ButtonDown(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		break;
	case WM_XBUTTONUP:
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
		{
			Input::SetKeyDownState(KeyCode::X1BUTTON, false);
			if (m_page->OnX1ButtonUp(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		else
		{
			Input::SetKeyDownState(KeyCode::X2BUTTON, false);
			if (m_page->OnX2ButtonUp(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		break;
	case WM_XBUTTONDBLCLK:
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
		{
			Input::SetKeyDownState(KeyCode::X1BUTTON, true); // The double click message is triggered on the second down click and will be followed by another BUTTONUP message
			if (m_page->OnX1ButtonDoubleClick(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		else
		{
			Input::SetKeyDownState(KeyCode::X2BUTTON, true); // The double click message is triggered on the second down click and will be followed by another BUTTONUP message
			if (m_page->OnX2ButtonDoubleClick(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
				return 0;
		}
		break;


	case WM_SIZE:
	{
		m_width = LOWORD(lParam);
		m_height = HIWORD(lParam);
		m_deviceResources->OnResize(m_height, m_width);

		m_viewport.Width = m_width;
		m_viewport.Height = m_height;

		m_scissorRect.right = static_cast<LONG>(m_width);
		m_scissorRect.bottom = static_cast<LONG>(m_height);

		if (m_page->OnWindowResized(m_height, m_width))
			return 0;
		break;
	}

	// Mouse Wheel
	case WM_MOUSEWHEEL:
		if (m_page->OnMouseWheel(static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)), static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;
		
	case WM_MOUSEHWHEEL:
		if (m_page->OnMouseHWheel(static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)), static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)), MouseButtonEventKeyStates(wParam)))
			return 0;
		break;


	// Keyboard Events
	case WM_CHAR:
		if (m_page->OnChar(static_cast<unsigned int>(wParam), static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;		
	case WM_KEYDOWN:
	{
		KeyCode keyCode = static_cast<KeyCode>(wParam);
		Input::SetKeyDownState(keyCode, true);

		// In the case of SHIFT, CTRL, MENU, there are LSHIFT and RSHIFT additional keycodes we need to set
		KeyCode keyCode2 = static_cast<KeyCode>(MapLeftRightKeys(wParam, lParam));
		if (keyCode != keyCode2)
			Input::SetKeyDownState(keyCode2, true);

		if (m_page->OnKeyDown(keyCode, static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;
	}
	case WM_KEYUP:
	{
		KeyCode keyCode = static_cast<KeyCode>(wParam); 
		Input::SetKeyDownState(keyCode, false); 

		// In the case of SHIFT, CTRL, MENU, there are LSHIFT and RSHIFT additional keycodes we need to set
		KeyCode keyCode2 = static_cast<KeyCode>(MapLeftRightKeys(wParam, lParam));
		if (keyCode != keyCode2)
			Input::SetKeyDownState(keyCode2, false);

		if (m_page->OnKeyUp(keyCode, static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;
	}
	case WM_SYSKEYDOWN:
	{
		KeyCode keyCode = static_cast<KeyCode>(wParam);
		Input::SetKeyDownState(keyCode, true);

		// In the case of SHIFT, CTRL, MENU, there are LSHIFT and RSHIFT additional keycodes we need to set
		KeyCode keyCode2 = static_cast<KeyCode>(MapLeftRightKeys(wParam, lParam));
		if (keyCode != keyCode2)
			Input::SetKeyDownState(keyCode2, true);

		if (m_page->OnSysKeyDown(keyCode, static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;
	}
	case WM_SYSKEYUP:
	{
		KeyCode keyCode = static_cast<KeyCode>(wParam);
		Input::SetKeyDownState(keyCode, false);

		// In the case of SHIFT, CTRL, MENU, there are LSHIFT and RSHIFT additional keycodes we need to set
		KeyCode keyCode2 = static_cast<KeyCode>(MapLeftRightKeys(wParam, lParam));
		if (keyCode != keyCode2)
			Input::SetKeyDownState(keyCode2, false);

		if (m_page->OnSysKeyUp(keyCode, static_cast<unsigned int>(LOWORD(lParam))))
			return 0;
		break;
	}


	// Window Events
	case WM_KILLFOCUS:		
		if (m_page->OnKillFocus())
			return 0;
		break;
		
	case WM_CREATE:
	{
		CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		m_height = cs->cy;
		m_width = cs->cx;
		m_deviceResources = std::make_shared<DeviceResources>(hWnd, m_width, m_height, m_title);

		m_viewport.Width = m_width;
		m_viewport.Height = m_height;

		m_scissorRect.right = static_cast<LONG>(m_width);
		m_scissorRect.bottom = static_cast<LONG>(m_height);

		m_renderer = std::make_unique<Renderer>(m_deviceResources, m_viewport, m_scissorRect);

		// Don't pass message to m_page, because InitializePage will not have been called yet
		return 0;
	}
	case WM_CLOSE:			
		if (m_page->OnWindowClosed())
			return 0;
		break;

	case WM_DPICHANGED:
		LOG_WARN("({0} - {1}: Received WM_DPICHANGED. Currently not handling  message", __FILE__, __LINE__); 
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
WPARAM Window::MapLeftRightKeys(WPARAM vk, LPARAM lParam)
{
	// This function taken from here: https://stackoverflow.com/questions/15966642/how-do-you-tell-lshift-apart-from-rshift-in-wm-keydown-events
	WPARAM new_vk = vk;
	UINT scancode = (lParam & 0x00ff0000) >> 16;
	int extended = (lParam & 0x01000000) != 0;

	switch (vk) {
	case VK_SHIFT:
		new_vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		break;
	case VK_CONTROL:
		new_vk = extended ? VK_RCONTROL : VK_LCONTROL;
		break;
	case VK_MENU:
		new_vk = extended ? VK_RMENU : VK_LMENU;
		break;
	default:
		// not a key we map from generic to left/right specialized just return it.
		break;
	}

	return new_vk;
}

void Window::PrepareToRun()
{
	// This function is required to run immediately before the Update/Render/Present loop
	// Upon initialization of DeviceResources, it will leave the command list in an open state
	// so that we can use it during initialization of the Page (and all controls). Therefore, we
	// must close it and execute its commands before we can start the rendering loop

	// Also perform other sanity checks
	if (m_page == nullptr)
		throw EXCEPTION("m_page was nullptr. Did you forget to call InitializeMainWindowPage<...>();");

	m_deviceResources->PrepareToRun();
}
void Window::Update(const Timer& timer) 
{
	// Must call deviceResources->Update() first because it will reset the commandlist so new commands can be issued
	m_deviceResources->Update();
	m_page->Update(timer);
	m_renderer->Update(timer, m_deviceResources->GetCurrentFrameIndex());
}
void Window::Render(const Timer& timer) 
{
	m_deviceResources->PreRender();
	m_page->Render();

	m_renderer->Render(m_deviceResources->GetCurrentFrameIndex());

	m_deviceResources->PostRender();
}
void Window::Present() 
{
	m_deviceResources->Present();
}

void Window::InitializeRenderer()
{
	m_camera = std::make_unique<Camera>();
	m_camera->LookAt({ 0.0f, 0.0f, -5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	m_camera->SetLens(static_cast<float>(0.25 * std::numbers::pi), 
		static_cast<float>(this->GetWidth()) / static_cast<float>(this->GetHeight()),
		1.0f, 1000.0f);

	auto il = std::vector<D3D12_INPUT_ELEMENT_DESC>{
		{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};


	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3); 
//	GeometryGenerator::MeshData box = geoGen.CreateSphere(1.0f, 20, 20);

	std::vector<std::uint16_t> crateIndices = box.GetIndices16();
	std::vector<CrateVertex> crateVertices(box.Vertices.size());
	for (unsigned int iii = 0; iii < box.Vertices.size(); ++iii)
	{
		crateVertices[iii].Pos = box.Vertices[iii].Position;
		crateVertices[iii].Normal = box.Vertices[iii].Normal;
		crateVertices[iii].TexC = box.Vertices[iii].TexC;
	}
	Mesh<CrateVertex> crateMesh(std::move(crateVertices), std::move(crateIndices));
	m_meshGroupCrate = std::make_unique<MeshGroup<CrateVertex>>(m_deviceResources, PRIMITIVE_TOPOLOGY::TRIANGLELIST);
	m_meshGroupCrate->PushBack(std::move(crateMesh));

	m_passConstantBufferCrate = std::make_unique<ConstantBufferMapped<CratePassConstants>>(m_deviceResources);
	m_passConstantBufferCrate->Update = [this](const Timer& timer, int frameIndex)
		{
			using namespace DirectX;

			CratePassConstants pc{};
			XMMATRIX view = m_camera->GetView();
			XMMATRIX proj = m_camera->GetProj();

			XMVECTOR viewDet = XMMatrixDeterminant(view);
			XMVECTOR projDet = XMMatrixDeterminant(proj);

			XMMATRIX viewProj = XMMatrixMultiply(view, proj);
			XMVECTOR viewProjDet = XMMatrixDeterminant(viewProj);

			XMMATRIX invView = XMMatrixInverse(&viewDet, view);
			XMMATRIX invProj = XMMatrixInverse(&projDet, proj);
			XMMATRIX invViewProj = XMMatrixInverse(&viewProjDet, viewProj);

			XMStoreFloat4x4(&pc.View, XMMatrixTranspose(view));
			XMStoreFloat4x4(&pc.InvView, XMMatrixTranspose(invView));
			XMStoreFloat4x4(&pc.Proj, XMMatrixTranspose(proj));
			XMStoreFloat4x4(&pc.InvProj, XMMatrixTranspose(invProj));
			XMStoreFloat4x4(&pc.ViewProj, XMMatrixTranspose(viewProj));
			XMStoreFloat4x4(&pc.InvViewProj, XMMatrixTranspose(invViewProj));
			pc.EyePosW = m_eyePosition;
			pc.RenderTargetSize = XMFLOAT2(static_cast<float>(this->GetWidth()), static_cast<float>(this->GetHeight()));
			pc.InvRenderTargetSize = XMFLOAT2(1.0f / static_cast<float>(this->GetWidth()), 1.0f / static_cast<float>(this->GetHeight()));
			pc.NearZ = 1.0f;
			pc.FarZ = 1000.0f;
			pc.TotalTime = timer.TotalTime();
			pc.DeltaTime = timer.DeltaTime();
			pc.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
			pc.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
			pc.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
			pc.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
			pc.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
			pc.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
			pc.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

			m_passConstantBufferCrate->CopyData(frameIndex, pc);
		};

	m_materialConstantBuffer = std::make_unique<ConstantBufferMapped<Material>>(m_deviceResources);
	m_materialConstantBuffer->Update = [this](const Timer& timer, int frameIndex)
		{
			using namespace DirectX;

			Material mat{};
			mat.DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			mat.FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
			mat.Roughness = 0.2f;

			m_materialConstantBuffer->CopyData(frameIndex, mat); 
		};

	m_objectConstantBuffer = std::make_unique<ConstantBufferMapped<ObjectData>>(m_deviceResources);
	m_objectConstantBuffer->Update = [this](const Timer& timer, int frameIndex)
		{
			using namespace DirectX;

			ObjectData data{};
			XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f) *
				XMMatrixScaling(1.0f, 1.0f, 1.0f) *
				XMMatrixRotationY(sinf(timer.TotalTime()));
			XMStoreFloat4x4(&data.World, XMMatrixTranspose(world)); 

			m_objectConstantBuffer->CopyData(frameIndex, data); 
		};

	m_sd0.Filter   = FILTER::MIN_MAG_MIP_POINT;
	m_sd0.AddressU = TEXTURE_ADDRESS_MODE::WRAP; 
	m_sd0.AddressV = TEXTURE_ADDRESS_MODE::WRAP;
	m_sd0.AddressW = TEXTURE_ADDRESS_MODE::WRAP;

	m_sd1.Filter = FILTER::MIN_MAG_MIP_POINT; 
	m_sd1.AddressU = TEXTURE_ADDRESS_MODE::CLAMP;
	m_sd1.AddressV = TEXTURE_ADDRESS_MODE::CLAMP;
	m_sd1.AddressW = TEXTURE_ADDRESS_MODE::CLAMP;

	m_sd2.Filter = FILTER::MIN_MAG_MIP_LINEAR;
	m_sd2.AddressU = TEXTURE_ADDRESS_MODE::WRAP;
	m_sd2.AddressV = TEXTURE_ADDRESS_MODE::WRAP;
	m_sd2.AddressW = TEXTURE_ADDRESS_MODE::WRAP;

	m_sd3.Filter = FILTER::MIN_MAG_MIP_LINEAR;
	m_sd3.AddressU = TEXTURE_ADDRESS_MODE::CLAMP;
	m_sd3.AddressV = TEXTURE_ADDRESS_MODE::CLAMP;
	m_sd3.AddressW = TEXTURE_ADDRESS_MODE::CLAMP;

	m_sd4.Filter = FILTER::ANISOTROPIC;
	m_sd4.AddressU = TEXTURE_ADDRESS_MODE::WRAP;
	m_sd4.AddressV = TEXTURE_ADDRESS_MODE::WRAP;
	m_sd4.AddressW = TEXTURE_ADDRESS_MODE::WRAP;
	m_sd4.MipLODBias = 0.0f;
	m_sd4.MaxAnisotropy = 8;

	m_sd5.Filter = FILTER::ANISOTROPIC;
	m_sd5.AddressU = TEXTURE_ADDRESS_MODE::CLAMP;
	m_sd5.AddressV = TEXTURE_ADDRESS_MODE::CLAMP;
	m_sd5.AddressW = TEXTURE_ADDRESS_MODE::CLAMP;
	m_sd5.MipLODBias = 0.0f;
	m_sd5.MaxAnisotropy = 8;



	Texture t1  = AssetManager::CheckoutTexture(m_deviceResources, "bricks.dds");
	Texture t2  = AssetManager::CheckoutTexture(m_deviceResources, "bricks2.dds");
	Texture t3  = AssetManager::CheckoutTexture(m_deviceResources, "bricks3.dds");
	Texture t4  = AssetManager::CheckoutTexture(m_deviceResources, "checkboard.dds");
	Texture t5  = AssetManager::CheckoutTexture(m_deviceResources, "grass.dds");
	Texture t6  = AssetManager::CheckoutTexture(m_deviceResources, "ice.dds");
	Texture t7  = AssetManager::CheckoutTexture(m_deviceResources, "stone.dds");
	Texture t8  = AssetManager::CheckoutTexture(m_deviceResources, "tile.dds");
	Texture t9  = AssetManager::CheckoutTexture(m_deviceResources, "water1.dds");
	Texture t10 = AssetManager::CheckoutTexture(m_deviceResources, "WireFence.dds");
	Texture t11 = AssetManager::CheckoutTexture(m_deviceResources, "WoodCrate01.dds");
	Texture t12 = AssetManager::CheckoutTexture(m_deviceResources, "WoodCrate02.dds");

	Texture t = t12;

	RenderPassSignature signature{
		TextureParameter{ 0 }, 
		ConstantBufferParameter{ 0 },
		ConstantBufferParameter{ 1 },
		ConstantBufferParameter{ 2 },
		SamplerParameter{ 0, &m_sd0 },
		SamplerParameter{ 1, &m_sd1 },
		SamplerParameter{ 2, &m_sd2 },
		SamplerParameter{ 3, &m_sd3 },
		SamplerParameter{ 4, &m_sd4 },
		SamplerParameter{ 5, &m_sd5 }
	};

	RenderPass& pass1 = m_renderer->EmplaceBackRenderPass(signature);
	SET_DEBUG_NAME(pass1, "Render Pass #1");

	pass1.BindConstantBuffer(2, m_passConstantBufferCrate.get());
	pass1.BindConstantBuffer(3, m_materialConstantBuffer.get());

	Shader vs = AssetManager::CheckoutShader("Crate-vs.cso", std::move(il));
	Shader ps = AssetManager::CheckoutShader("Crate-ps.cso");

	PipelineStateDesc psDesc{};
	psDesc.RootSignature = pass1.GetRootSignature();
	psDesc.VertexShader = vs;
	psDesc.PixelShader = ps;
	psDesc.SampleMask = UINT_MAX; /// ??? Why?
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = m_deviceResources->GetBackBufferFormat();
	psDesc.DSVFormat = m_deviceResources->GetDepthStencilFormat();

	RenderPassLayer& layer1 = pass1.EmplaceBackRenderPassLayer(m_meshGroupCrate.get(), psDesc);
	SET_DEBUG_NAME(layer1, "Render Pass Layer #1");

	RenderItem& crateRI = layer1.EmplaceBackRenderItem();
	SET_DEBUG_NAME(crateRI, "Crate RenderItem");

	crateRI.BindConstantBuffer(1, m_objectConstantBuffer.get());
	crateRI.BindTexture(0, t);






	/*
	std::vector<Vertex> squareVertices{
		{{ -0.5f, 0.5f, 0.5f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
		{{ 0.5f, 0.5f, 0.5f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
		{{ 0.5f, -0.5f, 0.5f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }},
		{{ -0.5f, -0.5f, 0.5f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }}
	};
	std::vector<std::uint16_t> squareIndices{ 0, 1, 3, 1, 2, 3 };
	Mesh<Vertex> mesh(std::move(squareVertices), std::move(squareIndices));

	std::unique_ptr<MeshGroup<Vertex>> meshGroup = std::make_unique<MeshGroup<Vertex>>(m_deviceResources);
	meshGroup->PushBack(std::move(mesh));

	std::vector<unsigned int> indices = { 0, 2 };
	MeshGroup<Vertex> meshGroupCopy = meshGroup->CopySubset(indices);
	m_meshGroup = std::make_unique<MeshGroup<Vertex>>(std::move(meshGroupCopy));
	SET_DEBUG_NAME_PTR(m_meshGroup, "MeshGroup");
	unsigned int meshIndex = 0;

	m_passConstantsBuffer = std::make_unique<ConstantBufferMapped<PassConstants>>(m_deviceResources);
	m_passConstantsBuffer->Update = [this](const Timer& timer, int frameIndex)
		{
			PassConstants pc{ static_cast<float>(this->GetWidth()), static_cast<float>(this->GetHeight()) };
			m_passConstantsBuffer->CopyData(frameIndex, pc);
		};

	RenderPassSignature sig{ 
		ConstantBufferDescription{ 0, m_passConstantsBuffer.get() } 
	};

	RenderPass& pass1 = m_renderer->EmplaceBackRenderPass(m_deviceResources, sig);
	SET_DEBUG_NAME(pass1, "Render Pass #1");




	const Shader& vs = AssetManager::GetShader("Control-vs.cso");
	const Shader& ps = AssetManager::GetShader("Control-ps.cso");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = vs.GetInputLayoutDesc();
	psoDesc.pRootSignature = pass1.GetRootSignature()->Get();
	psoDesc.VS = vs.GetShaderByteCode();
	psoDesc.PS = ps.GetShaderByteCode();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = false;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_deviceResources->GetBackBufferFormat();
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = m_deviceResources->GetDepthStencilFormat();

	psoDesc.DepthStencilState.DepthEnable = FALSE; 
	psoDesc.DepthStencilState.StencilEnable = FALSE; 

	RenderPassLayer& layer1 = pass1.EmplaceBackRenderPassLayer(m_deviceResources, m_meshGroup.get(), psoDesc, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	SET_DEBUG_NAME(layer1, "Render Pass Layer #1");

	RenderItem& squareRI = layer1.EmplaceBackRenderItem(meshIndex);
	SET_DEBUG_NAME(squareRI, "Square RenderItem"); 

	*/
}

#else
#error Only Supporting Windows
#endif
}