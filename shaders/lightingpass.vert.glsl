#version 330
layout(location=0) in vec4 in_position; // model-space
layout(location=1) in vec3 in_normal;   // model-space
layout(location=2) in vec2 in_texcoord;
layout(location=3) in vec3 in_tangent;
layout(location=4) in vec3 in_bitangent;

out vec2 vTexcoord;
out vec3 vPosition; // view-space
out vec3 vNormal;   // view-space
out vec3 vEyeDir;   // view-space
out vec4 vLightPosition;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;

uniform vec3 lightPos;

void main(void)
{
    gl_Position = mvpMatrix * in_position;
    
    vTexcoord = in_texcoord;
    
    vPosition = (viewMatrix * modelMatrix * in_position).xyz;
    
    vNormal = (normalMatrix * in_normal).xyz;
    
    vec3 tbnNormal    = normalize(vNormal);
    vec3 tbnTangent   = normalize(normalMatrix * in_tangent);
    //vec3 tbnBitangent = normalize(normalMatrix * in_bitangent);
    vec3 tbnBitangent  = normalize(cross(tbnNormal, tbnTangent));
    mat3 tbnMatrix = transpose(mat3(tbnTangent, tbnBitangent, tbnNormal));
    
    vec3 posInEyespace = (viewMatrix * modelMatrix * in_position).xyz;
    vEyeDir = vec3(0,0,0) - posInEyespace;

    vLightPosition = viewMatrix * vec4(lightPos, 0.0);

}