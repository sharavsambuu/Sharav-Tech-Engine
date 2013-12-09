#version 330
in vec2 vCoord;
uniform sampler2D canvasTexture;
out vec3 out_color;
void main(void)
{
    out_color = texture2D(canvasTexture, vCoord).xyz;
}