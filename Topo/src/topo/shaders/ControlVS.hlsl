

struct PerPassData
{
    float Width;
    float Height;
};

cbuffer cbPass : register(b0)
{
    PerPassData gPerPassData;
};

struct VertexIn
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    vout.Position = vin.Position;
    vout.Color = vin.Color;
	return vout;
}