#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitagent;

out vec2 TextCoord;
out vec4 origin;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clippingPlane;
uniform vec3 lightPos;

void main()
{
	vec4 worldPosition = model * vec4(aPos, 1.0);
	vec4 positionRelativeToCam = view * worldPosition;
	gl_ClipDistance[0] = dot(worldPosition, clippingPlane);
	gl_Position = projection * positionRelativeToCam;
	origin = vec4(view[0][2], view[1][2], view[2][2], 1.f);
	TextCoord = aTexCoord;

	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoords = TextCoord;
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

}