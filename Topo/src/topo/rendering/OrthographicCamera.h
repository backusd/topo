#pragma once
#include "topo/Core.h"
#include "topo/utils/MathHelper.h"


namespace topo
{
class OrthographicCamera
{
public:
	inline OrthographicCamera(float width, float height) noexcept
	{
		SetProjection(width, height);
	}
	OrthographicCamera(const OrthographicCamera&) noexcept = default;
	OrthographicCamera(OrthographicCamera&&) noexcept = default;
	OrthographicCamera& operator=(const OrthographicCamera&) noexcept = default;
	OrthographicCamera& operator=(OrthographicCamera&&) noexcept = default;

	// Projection
	inline void SetProjection(float width, float height) noexcept
	{
		DirectX::XMStoreFloat4x4(
			&m_proj,
			DirectX::XMMatrixOrthographicLH(width, height, 0.1f, 1000.0f)
		);
	}

	// Position
	ND inline DirectX::XMVECTOR GetPosition() const noexcept { return DirectX::XMLoadFloat3(&m_position); }
	ND constexpr const DirectX::XMFLOAT3& GetPosition3f() const noexcept { return m_position; }
	inline void SetPosition(float x, float y, float z) noexcept { m_position = { x, y, z }; UpdateViewMatrix(); }
	inline void SetPosition(const DirectX::XMFLOAT3& v) noexcept { m_position = v; UpdateViewMatrix(); }

	// Rotation
	ND constexpr float GetZRotation() const noexcept { return m_zRotation; }
	inline void SetZRotation(float zRotation) noexcept { m_zRotation = zRotation; UpdateViewMatrix(); }

	// Get View/Proj matrices.
	ND inline DirectX::XMMATRIX GetView() const noexcept { return DirectX::XMLoadFloat4x4(&m_view); }
	ND inline DirectX::XMMATRIX GetProj() const noexcept { return DirectX::XMLoadFloat4x4(&m_proj); }
	ND inline DirectX::XMMATRIX GetViewProj() const noexcept { return DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m_view), DirectX::XMLoadFloat4x4(&m_proj)); }
	ND inline DirectX::XMMATRIX GetViewInverse() const noexcept
	{
		DirectX::XMMATRIX view = GetView();
		DirectX::XMVECTOR viewDet = DirectX::XMMatrixDeterminant(view); 
		return DirectX::XMMatrixInverse(&viewDet, view);
	}
	ND inline DirectX::XMMATRIX GetProjInverse() const noexcept
	{
		DirectX::XMMATRIX proj = GetProj();
		DirectX::XMVECTOR projDet = DirectX::XMMatrixDeterminant(proj);
		return DirectX::XMMatrixInverse(&projDet, proj);
	}
	ND inline DirectX::XMMATRIX GetViewProjInverse() const noexcept
	{
		DirectX::XMMATRIX viewProj = GetViewProj();
		DirectX::XMVECTOR viewProjDet = DirectX::XMMatrixDeterminant(viewProj);
		return DirectX::XMMatrixInverse(&viewProjDet, viewProj);
	}

	ND constexpr const DirectX::XMFLOAT4X4& GetView4x4f() const noexcept { return m_view; }
	ND constexpr const DirectX::XMFLOAT4X4& GetProj4x4f() const noexcept { return m_proj; }
	ND inline DirectX::XMFLOAT4X4 GetViewProj4x4f() const noexcept
	{
		DirectX::XMFLOAT4X4 ret;
		DirectX::XMStoreFloat4x4(&ret, GetViewProj());
		return ret;
	}
	ND inline DirectX::XMFLOAT4X4 GetViewInverse4x4f() const noexcept
	{
		DirectX::XMFLOAT4X4 ret;
		DirectX::XMStoreFloat4x4(&ret, GetViewInverse());
		return ret;
	}
	ND inline DirectX::XMFLOAT4X4 GetProjInverse4x4f() const noexcept
	{
		DirectX::XMFLOAT4X4 ret;
		DirectX::XMStoreFloat4x4(&ret, GetProjInverse());
		return ret;
	}
	ND inline DirectX::XMFLOAT4X4 GetViewProjInverse4x4f() const noexcept
	{
		DirectX::XMFLOAT4X4 ret;
		DirectX::XMStoreFloat4x4(&ret, GetViewProjInverse());
		return ret;
	}

private:
	inline void UpdateViewMatrix() noexcept
	{
		DirectX::XMMATRIX view = DirectX::XMMatrixRotationZ(m_zRotation) *
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		DirectX::XMVECTOR viewDet = DirectX::XMMatrixDeterminant(view); 
		DirectX::XMStoreFloat4x4(
			&m_view,
			DirectX::XMMatrixInverse(&viewDet, view)
		);
	}

	DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 m_position = {};
	float m_zRotation = 0.0f;
};
}