#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(void){
	mat4 pvm = projection * view * model;
	//float d = sqrt(pvm[0][0] * pvm[0][0] + pvm[1][0] * pvm[1][0] + pvm[2][0] * pvm[2][0]);
	//d = sqrt( xx + yx² + zx² )

	//vec4 worldPosition = model * vec4(aPos,1.0f);
	//vec4 positionRelativeToCam = view * worldPosition;
	//gl_Position = projection * positionRelativeToCam;

	vec4 worldPosition = model * vec4(aPos,1.0f);
	mat3 invViewRot = inverse(mat3(view));
	vec3 pos = invViewRot * aPos;
	gl_Position = projection * view * model * vec4(pos, 1.0); 

	TexCoords = aTexCoord;
}