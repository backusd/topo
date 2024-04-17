 
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};
 
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
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
//    VertexOut vout;
//    vout.Position = vin.Position;
//    vout.Color = vin.Color;
//	return vout;
    
    
    VertexOut vout = (VertexOut) 0.0f;
    vout.Color = vin.Color;
	
    // Transform to world space.
    vin.Position.z = 1.0f;
    float4 posW = mul(vin.Position, gWorld);
//    vout.PosW = posW.xyz;

    // Transform to homogeneous clip space.
    vout.Position = mul(posW, gViewProj);
	

    return vout;
}