#version 330
layout(location=0) in vec3 in_position;

out vec2 vCoord;

void main(void)
{
    gl_Position = vec4(in_position, 1.0);
    vCoord = (in_position.xy+vec2(1,1))/2.0;
}