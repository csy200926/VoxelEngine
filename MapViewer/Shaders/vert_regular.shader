#version 410 core                                                 
                                                                  

layout(location = 0)in vec4 position;
layout(location = 1)in vec4 uv;
layout(location = 2)in vec4 color;
layout(location = 3)in vec3 normal;

out vec4 shadowCoord;
out vec4 vertColor;
out vec4 UV;
                        
uniform mat4 ModelToWorld_Matrix;
uniform mat4 WorldToView_Matrix;                                             
uniform mat4 Projective_Matrix;    
uniform mat4 DepthMVP_Matrix; 
     
void main(void)                                                     
{                                
	vec3 vectorToLight = vec3(0.693,0.712,-0.11);
	normalize(vectorToLight);

	gl_Position = Projective_Matrix 
				* WorldToView_Matrix 
				* ModelToWorld_Matrix 
				* position;

	shadowCoord = DepthMVP_Matrix * position;

	float simpleDiffuse = clamp(dot(vectorToLight, normal), 0.0, 1.0);
	simpleDiffuse = simpleDiffuse * 0.5 + 0.7f;
	vertColor = color * simpleDiffuse;

	UV = uv * 0.0625;

}                                               