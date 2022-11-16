#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec4 col;
out vec2 TextCoord;
out vec4 origin;

out vec4 clipSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	clipSpace = projection * view * model * aPos;
	gl_Position = clipSpace;
	origin = vec4(view[0][2], view[1][2], view[2][2], 1.f);
	col = aNormal;
	TextCoord = aTexCoord;
}