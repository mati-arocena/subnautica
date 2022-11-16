#version 330 core

out vec4 FragColor;
in vec4 col;
in vec2 TextCoord;
in vec4 origin;
in vec4 clipSpace;
uniform vec4 ourColor;
uniform float time;

uniform sampler2D texture_reflection;
uniform sampler2D texture_refraction;



void main()
{
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
    vec2 refractTextCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTextCoords = vec2(ndc.x, -ndc.y);

    vec4 refractColor = texture(texture_refraction, refractTextCoords);
    vec4 reflectColor = texture(texture_reflection, reflectTextCoords);
    //FragColor = mix( texture(texture0, TextCoord), texture(texture1, vec2(TextCoord.x * cos(time) + TextCoord.y * sin(time), -TextCoord.x * sin(time) + TextCoord.y * cos(time))), normalize(gl_FragCoord));
    //FragColor = abs(col);
    FragColor = mix(reflectColor, refractColor, 0.5);
}