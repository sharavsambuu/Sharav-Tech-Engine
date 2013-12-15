#version 330

uniform float     material_shininess; 
uniform vec3      material_diffuse_color;
uniform vec3      material_ambient_color;
uniform vec3      material_specular_color;
uniform vec3      meterial_emissive_color;

uniform sampler2D colour_texture;
uniform sampler2D normal_texture;
uniform sampler2D depth_texture;
uniform vec2 pixelSize;
uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColor;
uniform mat4 inversedProjectionView;
uniform vec3 cameraPos;

in vec2 vTexcoord;
in vec3 vPosition; // view-space
in vec3 vNormal;   // view-space
in vec3 vEyeDir;   // view-space

out vec4 outColor[2];

void main(void)
{
    vec3 pos = vec3((gl_FragCoord.x * pixelSize.x), 
                    (gl_FragCoord.y * pixelSize.y), 
                    0.0);
    pos.z = texture(depth_texture, pos.xy).r;
    vec3 normal = normalize(texture(normal_texture, pos.xy).xyz * 2.0 - 1.0);
    vec4 clip = inversedProjectionView * vec4(pos * 2.0 - 1.0, 1.0);
    pos = clip.xyz / clip.w;

    float dist = length(lightPos - pos);
    float attenuation = 1.0 - clamp(dist/lightRadius, 0.0, 1.0);

    if (attenuation==0.0) {
        discard;
    }
    
    vec3 incident = normalize(lightPos - pos);
    vec3 viewDir = normalize(cameraPos - pos);
    vec3 halfDir = normalize(incident + viewDir);

    float lambert = clamp(dot(incident, normal), 0.0, 1.0);
    float rFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
    float sFactor = pow(rFactor, 33.0);
    
    outColor[0] = vec4(lightColor.xyz * lambert * attenuation, 1.0);
    outColor[1] = vec4(lightColor.xyz * sFactor * attenuation * 0.33, 1.0);
}