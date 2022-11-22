#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec4 col;
out vec2 TextCoord;
out vec4 origin;
out vec3 toCameraVector;
out vec4 clipSpace;
out vec3 fromLightVector;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_position;
uniform vec3 lightPos;

const float fogDensity = 0.1;
const float fogGradient = 2.0;

void main()
{
	vec4 worldPosition = model * aPos;
	vec4 positionRelativeToCam = view * worldPosition;
	clipSpace = projection * positionRelativeToCam;
	gl_Position = clipSpace;
	origin = vec4(view[0][2], view[1][2], view[2][2], 1.f);
	col = aNormal;
	TextCoord = aTexCoord;
	toCameraVector = camera_position - worldPosition.xyz;
	fromLightVector = worldPosition.xyz - lightPos;
}