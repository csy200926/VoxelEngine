#version 420 core                                                 
                    
layout (binding = 0) uniform sampler2D renderedTexture;
			
in vec4 point_color;
in vec2 uvCoord;
	                                          
out vec4 color;                                                   
                                                                  
                                               
void main(void)                                                   
{             
	vec4 sampledColor = texture( renderedTexture, uvCoord);                                                    
	color = vec4(sampledColor);
} 