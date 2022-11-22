#version 330 core

in vec2 textCoord;
out vec4 FragColor;


uniform sampler2D occlusion_texture;
uniform sampler2D scene_texture;
uniform sampler2D scene_texture_depth;

uniform vec4 fog_color;
uniform vec4 water_fog_color;
uniform vec2 lightPos_SS;
uniform float time;
uniform float inside_water;

const float fogDensity = 0.1;
const float fogGradient = 3;
const int steps = 300;

float near = 0.1; 
float far  = 8.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{

    vec2 dir = lightPos_SS - textCoord;
    float step = length(dir) / steps;

    
    vec2 direction = normalize(dir);
    vec4 acc;

    for (int x = 0; x < steps; x++)
    {
        vec2 pos = textCoord + direction * step * x;
        acc += texture(occlusion_texture, pos);
    }
    acc /= steps;


    FragColor =  acc * inside_water * vec4(1.f,1.f, 1.f, 1.f) * 1.f + texture(scene_texture, textCoord);

    float dist = LinearizeDepth(texture(scene_texture_depth, textCoord).r) / far;

    FragColor = mix(FragColor, water_fog_color, dist * inside_water);
    //FragColor = texture(occlusion_texture, textCoord);
}