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
	float zDepth : POSITION;
};


float4 pixelShader(pixelInputType input) : SV_TARGET
{


	float4 color;
	

	float4 fakeShadow;
	
	color = diffuseColor;
	fakeShadow = float4 (input.zDepth, input.zDepth, input.zDepth, 1);
	
	fakeShadow += float4 (0.1f, 0.1f, 0.1f, 0.1f);
	
	color *= fakeShadow;


	//return float4 (0.9f, 0.2f, 0.3f, 1.0f);
	return color;
}