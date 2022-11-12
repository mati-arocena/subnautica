#version 330 core

layout (location = 0) in vec3 aPos;
out vec4 col;
uniform float time;
void main()
{
   gl_Position = vec4(sin(time) + aPos.x, -aPos.y, aPos.z, 1.0);
}
