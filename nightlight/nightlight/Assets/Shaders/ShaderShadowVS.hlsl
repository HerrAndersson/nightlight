cbuffer MatrixBuffer
{
	matrix wvp;
};

struct VS_IN
{
	float4 pos : POSITION;
};

// Shader to do vertex processing for camera view position and light view position.
float4 main(VS_IN input) : SV_POSITION
{
	float4 pos = input.pos;
	pos.w = 1.0f;
	pos = mul(pos, wvp);
	return pos;
}