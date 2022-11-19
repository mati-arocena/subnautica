#version 330 core

out vec4 FragColor;
in vec4 col;
in vec2 TextCoord;
in vec4 origin;
in vec4 clipSpace;
in vec3 toCameraVector;

uniform vec4 ourColor;
uniform float time;
uniform float refract_exp;
uniform vec3 normal;

uniform sampler2D texture_reflection;
uniform sampler2D texture_refraction;
uniform sampler2D dudv_map;

const float waveStrength = 0.02;

const float waveSpeed = 0.03;

void main()
{
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
    vec2 refractTextCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTextCoords = vec2(ndc.x, -ndc.y);

    float moveFactor = waveSpeed * time;
    moveFactor = mod(moveFactor,1);

    //vec2 distortion1 = (texture(dudv_map, vec2(TextCoord.x + moveFactor, TextCoord.y)).rg * 2.0 - 1.0) * waveStrength;
    //vec2 distortion2 = (texture(dudv_map, vec2(-TextCoord.x + moveFactor, TextCoord.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength;
    //vec2 totalDistortion = distortion1 + distortion2;

    vec2 distortedTexCoords = texture(dudv_map, vec2(TextCoord.x + moveFactor, TextCoord.y)).rg*0.1;
	distortedTexCoords = TextCoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
	vec2 totalDistortion = (texture(dudv_map, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    reflectTextCoords += totalDistortion;
    reflectTextCoords.x = clamp(reflectTextCoords.x, 0.001, 0.999);
    reflectTextCoords.y = clamp(reflectTextCoords.y, -0.999, -0.001);

    refractTextCoords += totalDistortion;
    refractTextCoords = clamp(refractTextCoords, 0.001, 0.999);

    vec4 refractColor = texture(texture_refraction, refractTextCoords);
    vec4 reflectColor = texture(texture_reflection, reflectTextCoords);

    vec3 viewVector = normalize(toCameraVector);
    float refractFactor = dot(viewVector, normal);
    refractFactor = pow(refractFactor, refract_exp);


    FragColor = mix(reflectColor, refractColor, refractFactor);

}