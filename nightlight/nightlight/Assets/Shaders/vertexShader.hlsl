cbuffer matrixBufferPerObject : register(cb0)
{
	matrix worldMatrix;
};

cbuffer matrixBufferPerFrame : register(cb1)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct vertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct vertexOutput
{
	float4 position : SV_POSITION0;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

	float3 worldPos : TEXCOORD1;
	//fakeShade
	float yDepth : POSITION;
};

vertexOutput vertexShader(vertexInputType input)
{
	vertexOutput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	
	output.worldPos = output.position;

	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.yDepth = 1;

	if (input.position.y < 0)
	{
		output.yDepth = 0;
	}

	return output;

}