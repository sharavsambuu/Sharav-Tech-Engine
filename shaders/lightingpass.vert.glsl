#version 330
layout(location=0) in vec3 in_position;

void main(void)
{
    gl_Position = vec4(in_position, 1.0);
}

/*
#version 330

layout(location=0) in vec4 in_position;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

void main(void)
{
    gl_Position = u_Projection * u_View * u_World * in_position;
}
*/
