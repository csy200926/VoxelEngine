#version 410 core                                                 
                                                                  
layout(location = 0)in vec4 position;
layout(location = 1)in vec4 uv;
layout(location = 2)in vec4 color;
layout(location = 3)in vec3 normal;

out vec4 point_color;
                        
uniform mat4 ModelToWorld_Matrix;
uniform mat4 WorldToView_Matrix;                                             
uniform mat4 Projective_Matrix;                                           
                                                                    
void main(void)                                                     
{                                

	vec4 viewPos = WorldToView_Matrix * position;
	gl_Position = Projective_Matrix * viewPos;

	float dis = (100.0 - length(viewPos.xyz)) / 100.0;
	
	if (dis > 0.7)
		dis = 1.0;

	if (dis < 0.5)
		dis = 0.2;

	point_color = vec4(dis);


}                                               