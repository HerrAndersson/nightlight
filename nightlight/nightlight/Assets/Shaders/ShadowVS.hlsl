cbuffer matrixBufferPerObject : register(cb0)
{
	matrix worldMatrix;
};

cbuffer matrixBufferPerFrame : register(cb1)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
};

struct VertexOutputType
{
	float4 position		: SV_POSITION;
};

VertexOutputType main(VertexInputType input)
{
	VertexOutputType OUT = (VertexOutputType)0;

	float4 output = input.position;
	output.w = 1.0f;

	output = mul(output, worldMatrix);
	output = mul(output, viewMatrix);
	output = mul(output, projectionMatrix);

	OUT.position = output;

	return OUT;
}