cbuffer matrixBufferPerBlendObject : register(cb0)
{
	matrix worldMatrix;
	int isSelected;
	float weight[4];
};

cbuffer matrixBufferPerFrame : register(cb1)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 camPos;
};

struct vertexInputType
{
	float4 position0	: POSITION0;
	float2 tex			: TEXCOORD0;
	float3 normal0		: NORMAL0;
	float3 position1	: POSITION1;
	float3 position2	: POSITION2;
	float3 position3	: POSITION3;
	float3 normal1		: NORMAL1;
	float3 normal2		: NORMAL2;
	float3 normal3		: NORMAL3;
};

struct vertexOutput
{
	float4 position		: SV_POSITION0;
	float2 tex			: TEXCOORD0;
	float3 normal		: NORMAL;

	float3 worldPos		: TEXCOORD1;
	float3 viewDir		: POSITION;
	int    isSelected   : SELECTED;
};

vertexOutput blendVertexShader(vertexInputType input)
{
	vertexOutput output;

	input.position0 *= weight[0];
	input.normal0 *= weight[0];

	input.position0.xyz += input.position1.xyz * weight[1];
	input.normal0 += input.normal1 * weight[1];
	input.position0.xyz += input.position2.xyz * weight[2];
	input.normal0 += input.normal2 * weight[2];
	input.position0.xyz += input.position3.xyz * weight[3];
	input.normal0 += input.normal3 * weight[3];

	input.position0.w = 1.0f;


	output.position = mul(input.position0, worldMatrix);

	output.worldPos = output.position;
	output.viewDir = camPos.xyz - output.worldPos.xyz;
	output.viewDir = normalize(output.viewDir);

	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;
	output.normal = mul(input.normal0, worldMatrix);
	output.normal = normalize(output.normal);

	output.isSelected = isSelected;

	return output;
}