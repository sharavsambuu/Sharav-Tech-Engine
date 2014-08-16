#version 330

precision highp float;
uniform sampler2D material_diffuse_texture;
uniform sampler2D material_normal_texture;
uniform sampler2D material_ambient_texture;
uniform sampler2D material_specular_texture;
uniform sampler2D material_opacity_texture;
uniform float     material_shininess; 
uniform vec3      material_diffuse_color;
uniform vec3      material_ambient_color;
uniform vec3      material_specular_color;
uniform vec3      material_emissive_color;
uniform int       hasDiffuseMap;
uniform int       hasNormalMap;
uniform int       hasAmbientMap;
uniform int       hasSpecularMap;
uniform int       hasOpacityMap;

in vec3 v_NormalVS;
in vec3 v_TangentVS;
in vec3 v_BitangentVS;
in vec2 v_TexCoord;

out vec4 out_color;  // albedo
out vec4 out_normal; // view space normal

vec4 encode(vec3 normal) 
{
    float p = sqrt(normal.z*8.0 + 8.0);
    return vec4(normal.xy/p + 0.5,0,0);
}

void main(void)
{
    vec3 diffuseColor  = texture2D(material_diffuse_texture , v_TexCoord).rgb;
    vec3 normal        = normalize(texture2D(material_normal_texture  , v_TexCoord).rgb*2.0 - 1.0);
    vec3 ambientColor  = texture2D(material_ambient_texture , v_TexCoord).rgb;
    vec3 specularColor = texture2D(material_specular_texture, v_TexCoord).rgb;
    vec3 opacityColor  = texture2D(material_opacity_texture , v_TexCoord).rgb;
    if (hasAmbientMap==0) ambientColor = diffuseColor;
    if (hasDiffuseMap==0) diffuseColor = material_diffuse_color;
    
    if (hasNormalMap != 0) {
        vec3 N = normalize(v_NormalVS);
        vec3 T = normalize(v_TangentVS);
        vec3 B = normalize(v_BitangentVS);
        mat3 TBN = mat3(T, B, N);
        normal = normalize(TBN * normal);
    } else {
        normal = v_NormalVS;
    }
   
    if (hasOpacityMap!=0){
        if (opacityColor==vec3(0,0,0))
            discard;
        else {
            out_color  = vec4(diffuseColor, 1.0);
            out_normal = encode(normal);
        }
    } else {
        out_color  = vec4(diffuseColor, 1.0);
        out_normal = encode(normal);       
    }
}