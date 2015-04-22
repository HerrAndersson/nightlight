cbuffer lightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

struct pixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 pixelShader(pixelInputType input) : SV_TARGET
{

	float4 color;
	color = diffuseColor;

	//return float4 (0.9f, 0.2f, 0.3f, 1.0f);
	return color;
}