#version 330

layout(location=0) in vec4 in_position;
layout(location=1) in vec3 in_normal;
layout(location=2) in vec2 in_texcoord;
layout(location=3) in vec3 in_tangent;
layout(location=4) in vec3 in_bitangent;

uniform mat4  u_Projection;
uniform mat4  u_View;
uniform mat4  u_World;

out vec3 v_NormalVS;
out vec3 v_TangentVS;
out vec3 v_BitangentVS;
out vec2 v_TexCoord;


void main(void)
{
    mat3 world3    = mat3(u_World);
    mat3 view3     = mat3(u_View);
    vec4 world_pos = u_World * in_position;
    vec4 view_pos  = u_View * world_pos;
    v_NormalVS     = view3 * world3 * in_normal;
    v_TangentVS    = view3 * world3 * in_tangent;
    v_BitangentVS  = view3 * world3 * in_bitangent;
    v_TexCoord     = in_texcoord;

    gl_Position = u_Projection * view_pos;
}