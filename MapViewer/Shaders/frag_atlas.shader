#version 420 core                                                 
                    
layout (binding = 0) uniform sampler2D renderedTexture;
layout (binding = 1) uniform sampler2D shadowMapTexture;
			
in vec4 shadowCoord;
in vec4 vertColor;
in vec4 UV;
	                                          
out vec3 color;                                                   
                                                                  
                                               
void main(void)                                                   
{   

	vec2 decodeUV = vec2(0,0);
	decodeUV.x = UV.x  + mod(UV.z ,0.0625);
	decodeUV.y = UV.y  + mod(UV.w ,0.0625);
		          
	vec4 sampledColor = texture( renderedTexture, decodeUV);                                                    
	
	vec3 projCoords = shadowCoord.xyz / shadowCoord.w; // matched to 0 - 1 already because of the bias matrix??

	float visibility = 1.0f;

	//TODO: why z value is not affected by bias matrix???
	if( texture(shadowMapTexture,projCoords.xy).r < projCoords.z - 0.001 )
		visibility = 0.5f;
	color = vertColor.xyz * sampledColor.xyz;
	color = clamp(sampledColor.xyz * vertColor.xyz - vertColor.w *vertColor.w * 0.05, 0.0, 1.0);

	color = color *visibility;
} 