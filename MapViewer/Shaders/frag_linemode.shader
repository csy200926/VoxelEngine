#version 420 core                                                 
                    
layout (binding = 0) uniform sampler2D renderedTexture;
			
in vec4 point_color;
	                                          
out vec3 color;                                                   
                                                                  
                                               
void main(void)                                                   
{   
	color = point_color.xyz;
} 