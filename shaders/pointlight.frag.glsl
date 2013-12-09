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

uniform float ambientIntensity;

//uniform vec3 lightPosition; // world-space
uniform vec3 spotDir;

in vec2 vTexcoord;
in vec3 vPosition; // world-space, view-space
in vec3 vNormal;   // view-space
in vec3 vEyeDir;   // view-space
in vec3 vLightDir; // view-space
in vec3 vLightPosition;

layout(location = 0) out vec3 out_color;


void main(void)
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec4 diffuseColor  = texture2D(material_diffuse_texture, vTexcoord);
    vec4 ambientColor  = texture2D(material_ambient_texture, vTexcoord);
    vec4 normalColor   = vec4(texture2D(material_normal_texture , vTexcoord).xyz, 1.0);
    vec4 specularColor = texture2D(material_specular_texture, vTexcoord);
    vec4 opacityColor  = texture2D(material_opacity_texture, vTexcoord);

    if (hasAmbientMap==0) {
        ambientColor = diffuseColor;
    }

    vec4 resAmbientColor  = vec4(0, 0, 0, 0);
    vec4 resDiffuseColor  = vec4(0, 0, 0, 0);
    vec4 resSpecularColor = vec4(0, 0, 0, 0);
    vec4 resultColor      = vec4(0, 0, 0, 0);

    
    vec3 normal = normalize(vNormal);
    if (hasNormalMap != 0) {
        vec3 nTex = normalColor.xyz;
        normal = nTex * 2.0 - 1.0;
        normal = normalize(normal);
    }
    vec3 lightDirection = normalize(vLightDir);

    // diffuse calculation, lambertian cosinus
    float lambertcos = max(0.0, dot(normal, lightDirection));
    if (hasDiffuseMap != 0) {
        resDiffuseColor = vec4((lightColor * diffuseColor.xyz).xyz, 1.0);
    } else {
        resDiffuseColor = vec4((lightColor * material_diffuse_color).xyz, 1.0);
    }

    // ambient calculation
    float ambientFraction = 0.25;
    //float ambientFraction = 1.0;
    if (hasAmbientMap != 0) {
        resAmbientColor = vec4((ambientFraction * lightColor * ambientColor.xyz).xyz, 1.0);
    } else {
        // resAmbientColor = vec4((ambientFraction * lightColor * material_ambient_color).xyz, 1.0);
        resAmbientColor = vec4((ambientFraction * lightColor * resDiffuseColor.xyz).xyz, 1.0);
    }

    // specular calculation, blinn-phong
    float lightDistance = length(vLightPosition-vPosition);
    vec3 halfVector = normalize(normalize(vEyeDir) + lightDirection);
    // Blinn-Phong needs shininiess about 4 * Phong shininess
    float specularcoeff = pow(max(0.0, dot(normal, halfVector)), 4.0 * material_shininess); 
    if (hasSpecularMap != 0) {
        resSpecularColor = vec4((specularcoeff * lightColor * specularColor.xyz).xyz, 1.0);
    } else {
        resSpecularColor = vec4((specularcoeff * lightColor * material_specular_color).xyz, 1.0);
    }

    float attenuation = 0.0;
    if (lambertcos>0.0) {
        float constant = 1.0;
        float linear   = 0.0014;
        float quadric  = 0.000007;
        attenuation = 1.0 / (constant + linear*lightDistance + quadric*lightDistance*lightDistance);
    } 
    //attenuation = 1.0;
    //if (hasSpecularMap!=1) {
    //    resSpecularColor = vec4(0.347, 0.347, 0.347, 1.0);
    //}

    //resultColor = clamp(resAmbientColor + attenuation *(resDiffuseColor + resSpecularColor), 0.0, 1.0);

   // if (lambertcos>0.0) {
        resultColor = clamp(resAmbientColor + attenuation * lambertcos * (resDiffuseColor + resSpecularColor), 0.0, 1.0);
   // } else {
   //     resultColor = clamp(resAmbientColor + attenuation * (resDiffuseColor + resSpecularColor), 0.0, 1.0);
   // }
    
    if (hasOpacityMap!=0){
        if (opacityColor.rgb==vec3(0,0,0))
            discard;
        else
            out_color = resultColor;
    } else {
        out_color = resultColor;
    }
    
}