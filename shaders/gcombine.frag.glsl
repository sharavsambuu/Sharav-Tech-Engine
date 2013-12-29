#version 330

uniform sampler2D diffuseTexture;
uniform sampler2D emissiveTexture;
uniform sampler2D specularTexture;
uniform sampler2D positionTexture;

uniform vec2 screen_dimension;

in vec2 vCoord;

out vec4 out_color;

void main(void)
{
    vec2 texCoord = vec2(gl_FragCoord.x, gl_FragCoord.y)/screen_dimension;
    vec3 diffuse = texture(diffuseTexture, texCoord).xyz;
    vec3 light   = texture(emissiveTexture, texCoord).xyz;
    vec3 specular = texture(specularTexture, texCoord).xyz;
    
    out_color.xyz = diffuse * 0.2; // ambient
    out_color.xyz += diffuse * light; // lambert
    out_color.xyz += specular; // specular
    out_color.a = 1.0;
    
    // debugging
    //out_color = texture2D(specularTexture, texCoord);// + texture2D(emissiveTexture, texCoord) + texture2D(specularTexture, texCoord);
    out_color = texture2D(positionTexture, texCoord);
}