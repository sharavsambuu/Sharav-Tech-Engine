#version 330
in vec2 vCoord;
uniform sampler2D uScreenTex;
uniform vec2      pixelSize;
uniform int       isVertical;
out vec3 out_color;

const float weights[5] = float[](0.12, 0.22, 0.32, 0.22, 0.12);

void main(void)
{
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
}