struct VertexOutputType
{
	float4 position : SV_POSITION;
	float  fDepth : TEXCOORD0;
};

float4  main(VertexOutputType IN) : SV_TARGET
{
	// Output the scene depth
	return float4(IN.fDepth, IN.fDepth, IN.fDepth, 1.0f);
}