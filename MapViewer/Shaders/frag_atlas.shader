#version 420 core                                                 
                    
layout (binding = 0) uniform sampler2D renderedTexture;
			
in vec4 point_color;
in vec4 UV;
	                                          
out vec3 color;                                                   
                                                                  
                                               
void main(void)                                                   
{   

	vec2 decodeUV = vec2(0,0);
	decodeUV.x = UV.x  + mod(UV.z ,0.0625);
	decodeUV.y = UV.y  + mod(UV.w ,0.0625);
		          
	vec4 sampledColor = texture( renderedTexture, decodeUV);                                                    
	
	
	color = sampledColor.xyz * point_color.xyz;
} 