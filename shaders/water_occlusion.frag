#version 330 core

out vec4 FragColor;
uniform sampler2D occlusion_map;
uniform sampler2D dudv_map;
in vec2 TextCoord;

uniform float time;


const float waveStrength = 0.02;
const float waveSpeed = 0.04;

void main()
{

    float moveFactor = waveSpeed * time;
    moveFactor = mod(moveFactor,1);

    vec2 distortedTexCoords = texture(dudv_map, vec2(TextCoord.x + moveFactor, TextCoord.y)).rg*0.1;
	distortedTexCoords = TextCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);

    FragColor = texture(occlusion_map, distortedTexCoords);
}