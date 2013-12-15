#version 330

uniform sampler2D diffuseTexture;
uniform sampler2D emissiveTexture;
uniform sampler2D specularTexture;

in vec2 vCoord;

out vec4 out_color;

void main(void)
{
    vec3 diffuse = texture(diffuseTexture, vCoord).xyz;
    vec3 light   = texture(emissiveTexture, vCoord).xyz;
    vec3 specular = texture(specularTexture, vCoord).xyz;
    
    out_color.xyz = diffuse * 0.2; // ambient
    out_color.xyz += diffuse * light; // lambert
    out_color.xyz += specular; // specular
    out_color.a = 1.0;
}