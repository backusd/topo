#pragma once
#include "topo/Core.h"


namespace topo
{
// Using Windows only macro here because there are so many references to DirectX
#ifdef TOPO_PLATFORM_WINDOWS

class GeometryGenerator
{
public:
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

	struct Vertex
	{
		constexpr Vertex() {}
		constexpr Vertex(
			const DirectX::XMFLOAT3& p,
			const DirectX::XMFLOAT3& n,
			const DirectX::XMFLOAT3& t,
			const DirectX::XMFLOAT2& uv) :
			Position(p),
			Normal(n),
			TangentU(t),
			TexC(uv) {}
		constexpr Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			Position(px, py, pz),
			Normal(nx, ny, nz),
			TangentU(tx, ty, tz),
			TexC(u, v) {}

		DirectX::XMFLOAT3 Position = {};
		DirectX::XMFLOAT3 Normal = {};
		DirectX::XMFLOAT3 TangentU = {};
		DirectX::XMFLOAT2 TexC = {};
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32> Indices32;

		ND constexpr std::vector<uint16>& GetIndices16()
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
					mIndices16[i] = static_cast<uint16>(Indices32[i]);
			}

			return mIndices16;
		}

	private:
		std::vector<uint16> mIndices16;
	};

	///<summary>
	/// Creates a box centered at the origin with the given dimensions, where each
	/// face has m rows and n columns of vertices.
	///</summary>
	ND static MeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions) noexcept;

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	ND static MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount) noexcept;

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	ND static MeshData CreateGeosphere(float radius, uint32 numSubdivisions) noexcept;

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
	ND static MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount) noexcept;

	ND static MeshData CreateArrow(float bottomCylinderRadius, float topCylinderRadius, float headRadius, float cylinderHeight, float headHeight, uint32 sliceCount, uint32 stackCount) noexcept;


	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	ND static MeshData CreateGrid(float width, float depth, uint32 m, uint32 n) noexcept;

	///<summary>
	/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
	///</summary>
	ND static MeshData CreateQuad(float x, float y, float w, float h, float depth) noexcept;

private:
	static void Subdivide(MeshData& meshData) noexcept;
	ND static Vertex MidPoint(const Vertex& v0, const Vertex& v1) noexcept;
	static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData) noexcept;
	static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData) noexcept;
};

#endif
}