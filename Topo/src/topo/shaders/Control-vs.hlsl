 

// Constant buffers can hold 4096 'vectors'. A vector is a float4
// 4096 / 5 = 819.2
#define MAX_INSTANCES 819

struct PerObjectData
{
    float4x4 World;
    float4 Color;
};

cbuffer cbPerObject : register(b0)
{
    PerObjectData gPerObjectData[MAX_INSTANCES];
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
    uint instanceID : SV_InstanceID;
};

struct VertexOut
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;
    vout.Color = gPerObjectData[vin.instanceID].Color;
	
    // Transform to world space.
    vin.Position.z = 1.0f;
    float4 posW = mul(vin.Position, gPerObjectData[vin.instanceID].World);

    // Transform to homogeneous clip space.
    vout.Position = mul(posW, gViewProj);


    return vout;
}