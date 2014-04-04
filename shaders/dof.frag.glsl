#version 330
in vec2 vCoord;

uniform sampler2D uScreenTex;
uniform sampler2D uDepthTex;
uniform vec2      pixelSize;
uniform int       isVertical;
uniform float     aspectRatio;
uniform float     focus;
uniform mat4      projectionInverseMatrix;

out vec3 out_color;

const float blurClamp = 3.0;
const float bias      = 0.6;

const float weights[5] = float[](0.12, 0.22, 0.32, 0.22, 0.12);

void main(void)
{
    float depth = texture2D(uDepthTex, vCoord).x;
    vec4 screenPosition = vec4(
        vCoord.x,
        vCoord.y,
        depth,
        1.0
    ) * 2.0 - 1.0;
    vec4 viewPosition = projectionInverseMatrix * screenPosition;
    float z = -(viewPosition.z / viewPosition.w);

    if (z < 50 || z>350) {
        vec2 values[5];
        if (isVertical==1) {
            values = vec2[](
                vec2(0.0, -pixelSize.y * 3),
                vec2(0.0, -pixelSize.y * 2),
                vec2(0.0,  pixelSize.y),
                vec2(0.0,  pixelSize.y * 2),
                vec2(0.0,  pixelSize.y * 3)
            );
        } else {
            values = vec2[](
                vec2(-pixelSize.y * 3, 0.0),
                vec2(-pixelSize.y * 2, 0.0),
                vec2( pixelSize.y    , 0.0),
                vec2( pixelSize.y * 2, 0.0),
                vec2( pixelSize.y * 3, 0.0)
            );
        }
        for (int i=0; i<5; i++) {
            vec3 temp = texture2D(uScreenTex, vCoord + values[i]).xyz;  
            out_color += temp * weights[i];
        }
    } else {
        out_color = texture2D(uScreenTex, vCoord).xyz;
    }    
}