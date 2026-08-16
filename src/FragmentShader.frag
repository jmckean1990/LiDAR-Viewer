#version 410 core
in vec3 vColor;
out vec4 FragColor;

uniform bool isPoint;

void main()
{
    if(isPoint) {
        vec2 coord = gl_PointCoord - vec2(0.5f);
        if(length(coord) > 0.5) discard;
    }
    
    FragColor = vec4(vColor, 1.0f);
};