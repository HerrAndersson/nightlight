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

	//door point light
	float3 lightPosPoint;
	float4 lightDiffusePoint;

	//player point light
	float3 lightPosPoint2;
	float4 lightDiffusePoint2;


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
	
	
	float3 viewDir : POSITION;

};


float4 pixelShader(pixelInputType input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	
	//initialize variables
	float3 reflection;
	float4 specular = float4(0.0f, 1.0f, 0.0f, 1.0f);
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	
	//normalize normals
	input.normal = normalize(input.normal);

	//sample the texture for diffuse
	float4 diffuse = AssetTexture.Sample(AssetSamplerState, input.tex);
	//use this to create ambient light
	float3 finalAmbient = diffuse * lightAmbientSpot;
	
	//calculate the point lights directions
	float3 pointLightDir = normalize(input.worldPos - lightPosPoint);
	float3 pointLightDir2 = normalize(input.worldPos - lightPosPoint2);
	
	float diffuseLighting = saturate(dot(input.normal, -pointLightDir));
	float diffuseLighting2 = saturate(dot(input.normal, -pointLightDir2));
	//add the two point lights
	

	//calculate fallof for point lights. 
	diffuseLighting *= (3) / dot(lightPosPoint - input.worldPos, lightPosPoint - input.worldPos);
	diffuseLighting2 *= (0.9) / dot(lightPosPoint2 - input.worldPos, lightPosPoint2 - input.worldPos);

	//calculate light to pixel vector for spotlight
	float3 lightToPixelVec = lightPosSpot - input.worldPos;

	//calculate vector lenght;
	float d = length(lightToPixelVec);

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

	//make sure the values are between 1 and 0, and add the ambient
	finalColor = saturate(finalColor + finalAmbient);

	finalColor += (diffuseLighting *  lightDiffusePoint);
	//add second pointlight
	//finalColor += (diffuseLighting2 *  lightDiffusePoint2);

	if (diffuseLighting > 0)
	{

		reflection = normalize(2 * diffuseLighting * input.normal + pointLightDir);
		reflection *= normalize(2 * diffuseLighting * input.normal + lightDirSpot);
		
		//Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDir)), 20);
		finalColor += (specular);
	}
	
	//Return Final Color
	return float4(finalColor, diffuse.a);
}