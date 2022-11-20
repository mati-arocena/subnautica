#version 330 core
layout (location = 0) in vec4 coord;

out vec2 textCoord;

void main()
{
	textCoord = coord.zw;
    gl_Position = vec4(coord.xy,0.0,1.0);
}