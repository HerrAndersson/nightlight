Texture2D AssetTexture;
SamplerState AssetSamplerState;

cbuffer lightBuffer
{
	float3 lightPosSpot;
	float  lightRangeSpot;
	float3 lightDirSpot;
	float  lightConeSpot;
	float3 lightAttSpot;
	float4 lightAmbientSpot;
	float4 lightDiffuseSpot;

	float3 lightPosPoint;
	float4 lightDiffusePoint;


};


cbuffer matrixBufferPerFrame
{
	matrix viewMatrix;
	matrix projectionMatrix;
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

	float4 diffuse = AssetTexture.Sample(AssetSamplerState, input.tex);

	//float4 ambientLight = (0.1f, 0.1f, 0.1f, 1.0f);

	input.normal = normalize(input.normal);

	//float4 diffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	float3 lightToPixelVec = lightPosSpot - input.worldPos;

	//distance between the light pos and pixel pos
	float d = length(lightToPixelVec);

	//ambient light
	float3 finalAmbient = diffuse * lightAmbientSpot;

//	//check if pixel to faaar
//	if (d > lightRangeSpot)
//		return float4(finalAmbient, diffuse.a);
//
	lightToPixelVec /= d;

	//Calculate how much light the pixel gets by the angle
	float howMuchLight = dot(lightToPixelVec, input.normal);

	if (howMuchLight > 0.0f)
	{
		//Add light to the finalColor of the pixel
		finalColor += diffuse * lightDiffuseSpot;

		//Calculate Light's Distance Falloff factor
		finalColor /= (lightAttSpot[0] + (lightAttSpot[1] * (d* d* d) / 4.5) + (lightAttSpot[2] * (d * d * d * d) / 4.5));

		//Calculate falloff from center to edge of pointlight cone
		finalColor *= pow(max(dot(-lightToPixelVec, lightDirSpot), 0.0f), lightConeSpot);
	}

	if (howMuchLight < 0.0f)
		finalColor = lightAmbientSpot;
	

	//make sure the values are between 1 and 0, and add the ambient
	finalColor = saturate(finalColor + finalAmbient);



		
	//float4 color = diffuse;
	float3 pointLightDir = normalize(input.worldPos - lightPosPoint);
	float diffuseLighting = saturate(dot(input.normal, -pointLightDir));
	diffuseLighting *= (50) / dot(lightPosPoint - input.worldPos, lightPosPoint - input.worldPos);
	//
	finalColor *= diffuseLighting;


	//Return Final Color
	return float4(finalColor, 1);

}