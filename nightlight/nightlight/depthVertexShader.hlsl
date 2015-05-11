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

float4 depthVertexShader(VertexInputType input) : SV_POSITION
{
	float4 output;
	
	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output = mul(input.position, worldMatrix);
	output = mul(output, viewMatrix);
	output = mul(output, projectionMatrix);

	return output;
}