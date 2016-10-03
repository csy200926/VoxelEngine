#version 410 core


layout(location = 0)in vec4 position;

uniform mat4 DepthMVP_Matrix;

void main(void)
{
	gl_Position = DepthMVP_Matrix * position;
}