cbuffer lightBuffer
{
	float4 position;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

struct pixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPos : TEXCOORD1;
	
	//fakeShade
	float yDepth : POSITION;

};


float4 pixelShader(pixelInputType input) : SV_TARGET
{
	float4 color;
	float4 fakeShadow;
	float4 ambientLight = (0.1f, 0.1f, 0.1f, 1.0f);

	color = diffuseColor;
	fakeShadow = float4 (input.yDepth, input.yDepth, input.yDepth, 1);
	fakeShadow += float4 (0.1f, 0.1f, 0.1f, 0.1f);
	
	//color *= fakeShadow;

	float3 lightDir = normalize(input.worldPos - position);
	float diffuseLighting = saturate(dot(input.normal, -lightDir));
	diffuseLighting *= (50) / dot(position - input.worldPos, position - input.worldPos);
	//
	color *= diffuseLighting;
	
	color *= ambientLight;

	return color;
}