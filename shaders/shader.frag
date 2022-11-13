#version 330 core

out vec4 FragColor;
in vec4 col;
in vec2 TextCoord;
in vec4 origin;
uniform vec4 ourColor;
uniform float time;

// uniform sampler2D texture0;
// uniform sampler2D texture1;


void main()
{
    // FragColor = mix( texture(texture0, TextCoord), texture(texture1, vec2(TextCoord.x * cos(time) + TextCoord.y * sin(time), -TextCoord.x * sin(time) + TextCoord.y * cos(time))), normalize(gl_FragCoord));
    FragColor = abs(col);
}