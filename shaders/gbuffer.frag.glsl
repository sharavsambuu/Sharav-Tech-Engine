#version 330
uniform sampler2D material_diffuse_texture;
uniform sampler2D material_normal_texture;
uniform sampler2D material_ambient_texture;
uniform sampler2D material_specular_texture;
uniform sampler2D material_opacity_texture;
uniform float     material_shininess; 
uniform vec3      material_diffuse_color;
uniform vec3      material_ambient_color;
uniform vec3      material_specular_color;
uniform vec3      meterial_emissive_color;

uniform int hasDiffuseMap;
uniform int hasOpacityMap;
uniform int hasAmbientMap;
uniform int hasNormalMap;
uniform int hasSpecularMap;

in vec2 vTexcoord;
in vec3 vPosition;
in vec3 vNormal;   
in vec3 vEyeDir;   

out vec4 out_color;
out vec4 out_normal;
out vec4 out_position;

void main(void)
{
    vec4 diffuseColor  = texture2D(material_diffuse_texture, vTexcoord);
    vec4 ambientColor  = texture2D(material_ambient_texture, vTexcoord);
    vec4 normalColor   = texture2D(material_normal_texture , vTexcoord);
    vec4 specularColor = texture2D(material_specular_texture, vTexcoord);
    vec4 opacityColor  = texture2D(material_opacity_texture, vTexcoord);

    if (hasAmbientMap==0) {
        ambientColor = diffuseColor;
    }

    if (hasDiffuseMap==0) {
        diffuseColor = vec4(material_diffuse_color, 1.0);
    }
    
    vec3 normal = normalize(vNormal);
    if (hasNormalMap != 0) {
        vec3 nTex = normalColor.xyz;
        normal = nTex * 2.0 - 1.0;
        normal = normalize(normal);
    }
   
    if (hasOpacityMap!=0){
        if (opacityColor.rgb==vec3(0,0,0))
            discard;
        else {
            out_color = diffuseColor;
            out_normal = vec4(normal, 1.0);
            out_position = vec4(vPosition, 1.0);
        }
    } else {
        out_color = diffuseColor;
        out_normal = vec4(normal, 1.0);
        out_position = vec4(vPosition, 1.0);
    }
}