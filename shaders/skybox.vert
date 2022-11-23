#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TextCoord;
out vec4 origin;

uniform mat4 view;
uniform mat4 projection;


void main(void){

	mat4 mat = view;
	mat[3] = vec4(0.0,0.0,0.0,1.0);
	
	gl_Position = projection * mat * vec4(aPos, 1.0); 
	TextCoord = aPos;
	
}