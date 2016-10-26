#version 420

in vec3 texcoords;
layout (binding = 0) uniform samplerCube cube_texture;
out vec4 frag_colour;

void main () {
	frag_colour = texture (cube_texture, texcoords);
	//frag_colour = vec4(texcoords, 1);// texture (cube_texture, texcoords);
}
