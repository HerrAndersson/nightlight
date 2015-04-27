cbuffer lightBuffer
{
	float3 lightPos;
	float lightRange;
	float3 lightDir;
	float lightCone;
	float3 lightAtt;
	float4 lightAmbient;
	float4 lightDiffuse;
	
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
//	float4 color;
//	float4 fakeShadow;
//	float4 ambientLight = (0.1f, 0.1f, 0.1f, 1.0f);
//
//	color = diffuseColor;
//	if (input.yDepth == 0)
//	{
//
//		color = float4(0.5f, 0.2f, 0.0f, 1.0f);
//	}
//
//
//	float3 lightDir = normalize(input.worldPos - position);
//	float diffuseLighting = saturate(dot(input.normal, -lightDir));
//	diffuseLighting *= (50) / dot(position - input.worldPos, position - input.worldPos);
//	//
//	color *= diffuseLighting;
//	
//	color *= ambientLight;

	input.normal = normalize(input.normal);

	float4 diffuse = float4(0.0f, 1.0f, 0.0f, 0.0f);

	float3 finalColor = float3(1.5f, 0.0f, 0.5f);

	float3 lightToPixelVec = lightPos - input.worldPos;

	//distance between the light pos and pixel pos
	float d = length(lightToPixelVec);

	//ambient light
	float3 finalAmbient = diffuse * lightAmbient;

	//check if pixel to faaar
	if (d > lightRange)
		return float4(finalAmbient, lightDiffuse.a);

	lightToPixelVec /= d;

	//Calculate how much light the pixel gets by the angle
	float howMuchLight = dot(lightToPixelVec, input.normal);

	
	if (howMuchLight > 0.0f)
	{
		//Add light to the finalColor of the pixel
		finalColor += diffuse * lightDiffuse;

		//Calculate Light's Distance Falloff factor
		finalColor /= (lightAtt[0] + (lightAtt[1] * d*d)) + (lightAtt[2] * (d*d*d*d));

		//Calculate falloff from center to edge of pointlight cone
		finalColor *= pow(max(dot(-lightToPixelVec, lightDir), 0.0f), lightCone);
	}

	else if (howMuchLight < 0.0f)
		finalColor = (0.0f, 0.0f, 0.0f);

	//make sure the values are between 1 and 0, and add the ambient
	finalColor = saturate(finalColor + finalAmbient);

	//Return Final Color
	return float4(finalColor, diffuse.a);

}