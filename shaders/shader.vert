#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoord;

out vec4 col;
out vec2 TextCoord;
out vec4 origin;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   origin = vec4(view[0][2], view[1][2], view[2][2], 1.f);
   col = vec4(aCol,1);
   TextCoord = aTexCoord;
}