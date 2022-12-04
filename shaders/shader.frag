#version 330 core


out vec4 FragColor;

in vec4 origin;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 TangentLightPos;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	vec4 FragPosLightSpace;
} fs_in;

//uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float time;

uniform float inside_water;


uniform vec3 difuse_color;
uniform vec3 specular_color;
uniform float specular_strenght;
uniform float specular_exponent;

uniform vec3 texture_factor; // DIF, SPEC, NOR
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform sampler2D occlusion_map;
uniform sampler2D dudv_map;
uniform sampler2D caustics_factor;
uniform sampler2D shadow_depth;

const float waveStrength = 0.02;

float avg(vec3 col)
{
    return (col.x + col.y + col.z) / 3;
}

float roughnessToSpec(float roughness)
{
    return 100/(500 * roughness + 0.01);
}

vec4 computeCaustics(vec3 ts_normal)
{
    #define CAUSTIC_SIZE 3              // Size of caustic
    #define CAUSTIC_FACTOR_SIZE 60      // Size of caustic patches
    #define CAUSTIC_SPEED 0.1f          // Caustic movement speed
    #define CAUSTIC_FACTOR_SPEED 0.5f   // Caustic patch movement speed

    #define CAUSTIC_INFLUENCE 4         // Caustic influence exponent. Higher -> Less caustics 

    float causticMoveFactor       = mod(CAUSTIC_SPEED * time, CAUSTIC_SIZE);
    float causticFactorMoveFactor = mod(CAUSTIC_FACTOR_SPEED * time, CAUSTIC_FACTOR_SIZE);

    vec2 causticCoord       = mod(fs_in.FragPos.xz, vec2(CAUSTIC_SIZE)) ;
    vec2 causticFactorCoord = mod(fs_in.FragPos.xz, vec2(CAUSTIC_FACTOR_SIZE)) ;
    
    vec2 distortedCausticsTexCoords = texture(dudv_map, vec2(fs_in.FragPos.x + causticMoveFactor, fs_in.FragPos.z)).rg*.1;
    vec2 distortedCausticsFactorTexCoords = texture(dudv_map, vec2(fs_in.FragPos.x + causticFactorMoveFactor, fs_in.FragPos.z)).rg*.1;

	distortedCausticsTexCoords =  causticCoord + vec2(distortedCausticsTexCoords.x, distortedCausticsTexCoords.y + causticMoveFactor);
	distortedCausticsFactorTexCoords =  causticFactorCoord + vec2(distortedCausticsFactorTexCoords.x, distortedCausticsFactorTexCoords.y + causticFactorMoveFactor);

	vec3 globalLightVector = normalize(-fs_in.TangentLightDir);
    
    float causticsFactor = texture(caustics_factor, distortedCausticsFactorTexCoords/CAUSTIC_FACTOR_SIZE).r;
    vec4 caustics = texture(occlusion_map, distortedCausticsTexCoords/CAUSTIC_SIZE) * pow(dot(ts_normal, globalLightVector), CAUSTIC_INFLUENCE) * smoothstep(0.3,.7,causticsFactor);

    return caustics;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz;

    if(projCoords.z > 1.0)
    {
       return 0.0;
    }  

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow_depth, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = abs(projCoords.z);
    // check whether current frag pos is in shadow

    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_depth, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_depth, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
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



	vec4 caustics = computeCaustics(normal);

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    FragColor = vec4(ambient +  (1 - shadow) * (diffuse + specular), 1.0) + (1 - shadow) * inside_water * caustics;
    //FragColor = vec4(vec3   (shadow), 1.0);
    //FragColor = vec4(vec3(normalize(vec3(fs_in.TangentViewPos))), 1.0);
}