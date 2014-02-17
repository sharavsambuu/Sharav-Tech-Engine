#version 330

uniform float     material_shininess; 
uniform vec3      material_diffuse_color;
uniform vec3      material_ambient_color;
uniform vec3      material_specular_color;
uniform vec3      meterial_emissive_color;

uniform sampler2D colour_texture;
uniform sampler2D normal_texture;
uniform sampler2D depth_texture;
uniform sampler2D position_texture;

uniform vec2 pixelSize;
uniform vec4 lightColor;
uniform float lightRadius;
uniform mat4 inverseProjection;
uniform vec3 cameraPos;
uniform vec2 screen_dimension;
uniform vec3 lightPos;

in vec2 vTexcoord;
in vec3 vPosition; // view-space
in vec3 vNormal;   // view-space
in vec3 vEyeDir;   // view-space
in vec4 vLightPosition; // light position in view-space

out vec4 out_color1; // emissive
out vec4 out_color2; // specular

void main(void)
{
    vec2 texCoord = vec2(gl_FragCoord.x, gl_FragCoord.y)/screen_dimension;
    
    float depth = texture2D(depth_texture, texCoord);
/*    vec4 screenPosition = vec4(
        texCoord,
        depth,
        1.0
    )*2.0 - 1.0;
    vec4 positionInView = inverseProjection * screenPosition;
    positionInView /= positionInView.w; // surface position in view-space
*/
    vec4 positionInWorld = texture2D(position_texture, texCoord);
    
    //vec3 normal = normalize(texture2D(normal_texture, texCoord).xyz*2.0-1.0); // normal in view-space
    vec3 normal = normalize(texture2D(normal_texture, texCoord).xyz); // normal in view-space
    //float distance = length(vec3(vLightPosition.xyz - positionInView.xyz));
    //float distance2surface = distance(positionInWorld.xyz, lightPos);
    float distance2surface = length(vec3(vLightPosition.xyz - positionInWorld.xyz));

    vec3 lightDirection = normalize(positionInWorld.xyz - lightPos);
    
    //vec4 color = vec4(1.0, 1.0, 1.0, 1.0) * 0.8 * dot(normal, -lightDirection);
    //float attenuation = 0.0 + 0.008 * distance2surface + 0.002 * distance2surface * distance2surface;

    /*if (distance > lightRadius) {
        out_color1 = vec4(1.0,0.5,0.5,1.0);
        out_color2 = vec4(1.0,0.5,0.5,1.0);
    }*/

    float isInRadius = step(0.0, lightRadius - distance2surface);

    //float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);
    float attenuation = 0.0;
    float constant = 1.0;
    float linear   = 0.0014;
    float quadric  = 0.000007;
    //attenuation = 1.0 / (constant + linear*distance + quadric*distance*distance);
    //attenuation = lightRadius - clamp(distance, 0.0, lightRadius);
    //attenuation = 1.0 / (constant + linear*distance2surface + quadric*distance2surface*distance2surface);
    attenuation = distance2surface/(1-(distance2surface/lightRadius)*(distance2surface/lightRadius));
    attenuation = attenuation / lightRadius + 1;
    attenuation = 1.0/(attenuation*attenuation);
    
    //vec3 incidentDir = normalize(vLightPosition.xyz - positionInView.xyz);
    vec3 incidentDir = -lightDirection;
    vec3 viewDir = normalize(vEyeDir);
    vec3 halfDir = normalize(incidentDir + viewDir);
    
    float lambert = 0.0; // diffuse intensity
    float sFactor = 0.0; // specular intensity

    //if (attenuation > 0.0) {
    lambert = clamp(dot(incidentDir, normal), 0.0, 1.0);
    float rFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
    if (rFactor > 0.0)
        sFactor = pow(rFactor, 33.0);    
    //}    
/*
    if (length(-viewDir * lightRadius + lightPos) < depth)
        discard;
*/
    out_color1 = vec4(lightColor.xyz * lambert * attenuation * isInRadius, 1.0);
    out_color2 = vec4(lightColor.xyz * sFactor * attenuation * 0.33, 1.0);

}