#version 330 core

out vec4 FragColor;

in vec2 TextCoord;
in vec4 origin;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float time;


uniform vec3 difuse_color;
uniform vec3 specular_color;
uniform float specular_strenght;
uniform float specular_exponent;

uniform vec3 texture_factor; // DIF, SPEC, NOR
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;


float avg(vec3 col)
{
    return (col.x + col.y + col.z) / 3;
}

float roughnessToSpec(float roughness)
{
    return 100/(500 * roughness + 0.01);
}


void main()
{
    vec3 textNormal = texture(texture_normal, fs_in.TexCoords).rgb;
    vec3 normal = texture_factor.z * normalize(textNormal * 2.0 - 1.0) + (1 - texture_factor.z) * fs_in.Normal; 

    vec3 color = texture_factor.x * texture(texture_diffuse, fs_in.TexCoords).rgb + (1-texture_factor.x) * difuse_color;
       
    vec3 ambient = 0.1 * color;
    
    //difusse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * lightColor;

    FragColor = vec4(ambient + diffuse, 1.0);

    //specular
    
    /*
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specular_exponent);

    vec3 specularColor = texture(texture_specular, fs_in.TexCoords).rgb * texture_factor.y + (1 - texture_factor.y) * specular_color;
    vec3 specular = specular_strenght * spec * specularColor;
    */

    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float roughness = avg(texture(texture_specular, fs_in.TexCoords).rgb);
    float spec = texture_factor.y * roughnessToSpec(roughness) + (1 - texture_factor.y) * pow(max(dot(viewDir, reflectDir), 0.0), specular_exponent);

	
    vec3 specularColor = texture(texture_specular, fs_in.TexCoords).rgb * texture_factor.y + (1 - texture_factor.y) * specular_color;
    
    vec3 specular = specular_strenght * spec * specularColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}