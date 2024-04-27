#pragma once
#include "topo/Core.h"
#include "Renderer.h"
#include "OrthographicCamera.h"
#include "AssetManager.h"



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

	struct UIPassConstants
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

	struct UIObjectData
	{
		DirectX::XMFLOAT4X4 World;
	};
	struct ObjectData
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
	};





class UIRenderer
{
public:
	inline UIRenderer(float windowWidth, float windowHeight) noexcept :
		m_renderer(),
		m_orthographicCamera(windowWidth, windowHeight),
		m_windowWidth(windowWidth),
		m_windowHeight(windowHeight)
	{
		m_orthographicCamera.SetPosition(windowWidth / 2, -1 * windowHeight / 2, 0.0f);

		m_renderer.SetViewport({ 0.0f, 0.0f, windowWidth, windowHeight, 0.0f, 1.0f });
		m_renderer.SetScissorRect({ 0, 0, static_cast<LONG>(windowWidth), static_cast<LONG>(windowHeight) });
	}
	UIRenderer(UIRenderer&&) = delete;
	UIRenderer(const UIRenderer&) = delete;
	UIRenderer& operator=(UIRenderer&&) = delete;
	UIRenderer& operator=(const UIRenderer&) = delete;

	inline void OnWindowResize(float width, float height) noexcept
	{
		m_windowWidth = width;
		m_windowHeight = height;

		m_renderer.SetViewport({ 0.0f, 0.0f, width, height, 0.0f, 1.0f });
		m_renderer.SetScissorRect({ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) });

		m_orthographicCamera.SetProjection(width, height);
		m_orthographicCamera.SetPosition(width / 2, -1 * height / 2, 0.0f);
	}

	void SetDeviceResources(std::shared_ptr<DeviceResources> deviceResources);

	void Update(const Timer& timer, int frameIndex);
	inline void Render(int frameIndex) { m_renderer.Render(frameIndex); }

	void InitializeRenderer();
	ND constexpr float GetWindowWidth() const noexcept { return m_windowWidth; }
	ND constexpr float GetWindowHeight() const noexcept { return m_windowHeight; }


	void DrawRectangle(float left, float top, float right, float bottom);
	void DrawLine(float x1, float y1, float x2, float y2, float thickness = 1.0f);



private:
	Renderer			m_renderer;
	OrthographicCamera	m_orthographicCamera;
	std::shared_ptr<DeviceResources> m_deviceResources = nullptr;
	float m_windowWidth;
	float m_windowHeight;

	// vector of world matrices
	std::vector<UIObjectData> m_rectangleRenderItemTransforms;
	const unsigned int m_opaqueLayerIndex = 0;
	const unsigned int m_rectangleRenderItemIndex = 0;

	// 2D Test
	std::unique_ptr<ConstantBufferMapped<UIPassConstants>>	m_uiPassConstantsBuffer = nullptr;
	std::unique_ptr<MeshGroup<Vertex>> m_meshGroup = nullptr;
	std::unique_ptr<ConstantBufferMapped<UIObjectData>> m_uiObjectConstantBuffer = nullptr;
	DirectX::XMFLOAT3 m_eyePosition = {};
};


}