struct VertexInput
{
    float2 position : POSITION;
    float2 uv : TEXCOORD;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

cbuffer BufferData : register(b0)
{
    float4x4 ViewProjection;
    float4x4 Projection;
    float2 Resolution;
    float Time;
}

#define TO_SRGB(_Linear_Color) (pow(_Linear_Color, 1.0 / 2.2))
#define TO_LINEAR(_SRGB_Color) (pow(_SRGB_Color, 2.2))