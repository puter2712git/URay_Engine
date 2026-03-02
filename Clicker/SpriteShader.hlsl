Texture2D texture0 : register(t0);
SamplerState samplerState : register(s0);

cbuffer passes : register(b0)
{
	float4x4 viewProj;
}

cbuffer objects : register(b1)
{
	float4x4 world;
}

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

PS_INPUT mainVS(VS_INPUT input)
{
	PS_INPUT output;
	
	float4 worldPos = mul(float4(input.position, 1.0f), world);
	output.position = mul(worldPos, viewProj);
	output.color = input.color;
	output.uv = input.uv;
	
	return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
	float4 color = texture0.Sample(samplerState, input.uv);
	return color;
}
