#version 330 core

out vec4 FragColor;
in vec4 col;
in vec2 TextCoord;
in vec4 origin;
in vec4 clipSpace;
in vec3 toCameraVector;
in vec3 fromLightVector;
in float visibility;

uniform vec4 ourColor;
uniform float time;
uniform float refract_exp;
uniform vec3 normal_direction;

uniform sampler2D texture_reflection;
uniform sampler2D texture_refraction;
uniform sampler2D texture_refraction_depth;
uniform sampler2D dudv_map;
uniform sampler2D normal_map;
uniform vec3 lightColor;
uniform vec4 fog_color;
uniform vec4 water_fog_color;
uniform float inside_water;

const float waveStrength = 0.02;
const float waveSpeed = 0.04;
const float shininess = 20.0;
const float reflectivity = 0.6;

const float near = 0.1; 
const float far = 6.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
    vec2 refractTextCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTextCoords = vec2(ndc.x, -ndc.y);

    float moveFactor = waveSpeed * time;
    moveFactor = mod(moveFactor,1);

    float floorDistance = LinearizeDepth(texture(texture_refraction_depth, refractTextCoords).r) / far;
    float waterDistance = LinearizeDepth(gl_FragCoord.z) / far;
    float dist = floorDistance - waterDistance;    

    vec2 distortedTexCoords = texture(dudv_map, vec2(TextCoord.x + moveFactor, TextCoord.y)).rg*0.1;
	distortedTexCoords = TextCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(dudv_map, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(dist/20.0, 0.0, 1.0);

    reflectTextCoords += totalDistortion;
    reflectTextCoords.x = clamp(reflectTextCoords.x, 0.001, 0.999);
    reflectTextCoords.y = clamp(reflectTextCoords.y, -0.999, -0.001);

    refractTextCoords += totalDistortion;
    refractTextCoords = clamp(refractTextCoords, 0.001, 0.999);

    vec4 refractColor = texture(texture_refraction, refractTextCoords);
    vec4 reflectColor = texture(texture_reflection, reflectTextCoords);

    vec3 viewVector = normalize(toCameraVector);
    float refractFactor = dot(viewVector, normal_direction);
    refractFactor = pow(refractFactor, refract_exp);

    vec4 normalMapColor = texture(normal_map, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
    normal = normalize(normal); // Global space

    vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shininess);
	vec3 specularHighlights = lightColor * specular * reflectivity * clamp(dist/20.0, 0.0, 1.0);

    FragColor = mix(FragColor, water_fog_color, 0.35 * (1-inside_water)) + vec4(specularHighlights, .1);
    refractColor = mix(refractColor, FragColor, clamp(dist * (1-inside_water), 0.0, 1.0));

    FragColor = mix(reflectColor, refractColor, refractFactor);
    FragColor = mix(FragColor, water_fog_color, 0.35 * (1-inside_water)) + vec4(specularHighlights, .1);
    FragColor.a = clamp(dist/5.0, 0.0, 1.0);
}