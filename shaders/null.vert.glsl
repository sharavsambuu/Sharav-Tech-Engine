#version 330
layout(location=0) in vec4 in_position; // model-space
layout(location=1) in vec3 in_normal;   // model-space
layout(location=2) in vec2 in_texcoord;
layout(location=3) in vec3 in_tangent;
layout(location=4) in vec3 in_bitangent;

uniform mat4 mvpMatrix;

void main(void)
{
    gl_Position = mvpMatrix * in_position;
}