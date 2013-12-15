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
uniform vec4 lightColor;
uniform float lightRadius;
uniform mat4 inverseProjection;
uniform vec3 cameraPos;
uniform vec2 screen_dimension;

in vec2 vTexcoord;
in vec3 vPosition; // view-space
in vec3 vNormal;   // view-space
in vec3 vEyeDir;   // view-space
in vec4 vLightPosition; // light position in view-space

out vec4 out_color1;
out vec4 out_color2;

void main(void)
{
    vec2 texCoord = vec2(gl_FragCoord.x, gl_FragCoord.y)/screen_dimension;
    
    float depth = texture2D(depth_texture, texCoord);
    vec4 screenPosition = vec4(
        texCoord,
        depth,
        1.0
    ) * 2.0 - 1.0;
    vec4 positionInView = inverseProjection * screenPosition;    
    positionInView = positionInView / positionInView.w; // surface position in view-space
    vec3 normal = normalize(texture2D(normal_texture, texCoord).xyz); // normal in view-space

    float distance = length(vLightPosition - positionInView);
    float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);
    //if (attenuation == 0.0)
    //    discard;
    attenuation = 1.0;

    vec3 incidentDir = normalize((vLightPosition - positionInView).xyz);
    vec3 viewDir = normalize(vEyeDir);
    vec3 halfDir = normalize(incidentDir + viewDir);
    
    float lambert = clamp(dot(incidentDir, normal), 0.0, 1.0);
    float rFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
    float sFactor = pow(rFactor, 33.0);

    
    out_color1 = vec4(lightColor.xyz * lambert * attenuation, 1.0);
    out_color2 = vec4(lightColor.xyz * sFactor * attenuation * 0.33, 1.0);
    
}