Texture2D AssetTexture;
SamplerState AssetSamplerState;

cbuffer lightBuffer : register(cb0)
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

struct pixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

	float3 worldPos : TEXCOORD1;
	float3 viewDir : POSITION;
	float3 colorModifier : COLORMODIFIER;
};


float4 pixelShader(pixelInputType input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	
	float3 reflection;
	float4 specular = float4(0.0f, 1.0f, 0.0f, 1.0f);
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	//sample the texture for diffuse
	float4 diffuse = AssetTexture.Sample(AssetSamplerState, input.tex);

	float3 finalAmbient = diffuse * lightAmbientSpot;
	
	//calculate the point lights directions
	float3 pointLightDir = normalize(input.worldPos - lightPosPoint);
	float3 pointLightDir2 = normalize(input.worldPos - lightPosPoint2);
	
	float3 diffuseLighting = saturate(dot(input.normal, -pointLightDir));
	float3 diffuseLighting2 = saturate(dot(input.normal, -pointLightDir2));

	//add the two point lights
	//calculate fallof for point lights. 
	diffuseLighting *= (1) / dot(lightPosPoint - input.worldPos, lightPosPoint - input.worldPos);
	diffuseLighting2 *= (5) / dot(lightPosPoint2 - input.worldPos, lightPosPoint2 - input.worldPos);

	//calculate light to pixel vector for spotlight
	float3 lightToPixelVec = lightPosSpot - input.worldPos;

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

	finalColor = saturate(finalColor + finalAmbient);
	finalColor += (diffuseLighting *  lightDiffusePoint);
	finalColor += (diffuse *(diffuseLighting2 * lightDiffusePoint2));

	if (diffuseLighting.x > 0 || diffuseLighting.y > 0 || diffuseLighting.z > 0)
	{
		reflection = normalize(2 * diffuseLighting * input.normal + pointLightDir);
		reflection = normalize(2 * diffuseLighting2 * input.normal + pointLightDir2);
		reflection *= normalize(2 * diffuseLighting * input.normal + lightDirSpot);
		
		//Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDir)), 20);
		finalColor += (specular);
	}
	
	//Return Final Color
	return float4(finalColor += input.colorModifier, diffuse.a);
}