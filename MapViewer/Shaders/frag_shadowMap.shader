#version 410 core

// Ouput data
layout(location = 0) out float fragmentdepth;
//out vec3 color;     
void main(){
	//color = vec3(1.0);
	// Not really needed, OpenGL does it anyway
	fragmentdepth = gl_FragCoord.z;
}