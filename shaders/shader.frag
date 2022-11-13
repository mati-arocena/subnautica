#version 330 core

out vec4 FragColor;
in vec4 col;
in vec2 TextCoord;
in vec4 origin;


in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec4 ourColor;
uniform float time;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform sampler2D texture_height;

void main()
{
    vec3 normal = texture(texture_normal, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 color = texture(texture_diffuse, fs_in.TexCoords).rgb;
       
    vec3 ambient = 0.1 * color;
    
    //difusse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * lightColor;

    FragColor = vec4(ambient + diffuse, 1.0);   
}