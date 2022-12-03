#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitagent;

layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TextCoord;
out vec4 origin;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	vec4 FragPosLightSpace;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 clippingPlane;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool has_animation;
uniform mat4 light_space_matrix;

void main()
{	
	vec4 totalPosition = vec4(0.0f);
	if (has_animation)
	{
		for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
		{
			if(boneIds[i] == -1) 
				continue;
			if(boneIds[i] >=MAX_BONES) 
			{
				totalPosition = vec4(aPos,1.0f);
				break;
			}
			vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f);
			totalPosition += localPosition * weights[i];
			vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
		}
	} 
	else
	{
		totalPosition = vec4(aPos,1.);
	}
	

	//vec4 worldPosition = model * vec4(aPos,1.0f);
	vec4 worldPosition = model * totalPosition;
	vec4 positionRelativeToCam = view * worldPosition;
	gl_ClipDistance[0] = dot(worldPosition, clippingPlane);
	gl_Position = projection * positionRelativeToCam;

	TextCoord = aTexCoord;

	vs_out.FragPos = vec3(worldPosition);
	vs_out.TexCoords = TextCoord;
	vs_out.FragPosLightSpace = light_space_matrix * worldPosition;
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	
	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;
	// Tangent space normal from local space
	vs_out.Normal = TBN * N;

	vs_out.TangentViewPos = TBN * viewPos;
}