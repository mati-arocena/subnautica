#version 330 core

out vec4 FragColor;

uniform sampler2D scene_texture_depth;


void main()
{
	FragColor = vec4(vec3(0.f),1.0);
}