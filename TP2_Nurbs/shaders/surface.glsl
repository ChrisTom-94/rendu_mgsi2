#type vertex
#version 450

layout(location = 0) in vec4 a_PositionAndCurvature;

layout (std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

struct VertexOutput
{
    float Curvature;
};

layout(location = 0) out VertexOutput Output;

void main()
{
    Output.Curvature = a_PositionAndCurvature.w;
    gl_Position = u_ViewProjection * vec4(a_PositionAndCurvature.xyz, 1.0);
}


#type fragment
#version 450

struct VertexOutput
{
    float Curvature;
};

layout(location = 0) in VertexOutput Input;

layout(location = 0) out vec4 color;

uniform float u_ShowCurvatureMap;

void main()
{
    if (u_ShowCurvatureMap == 0.0) {
        color = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }

    float epsilon = 0.00005;

    float curvature = Input.Curvature;

    vec3 colorVec;
    vec3 blue = vec3(0.0, 0.0, 1.0);
    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 black = vec3(0.0, 0.0, 0.0);

    if (curvature < -epsilon) {
        colorVec = mix(blue, black, smoothstep(-1.0, -epsilon, curvature));
    } else if (curvature > epsilon) {
        colorVec = mix(black, red, smoothstep(epsilon, 1.0, curvature));
    } else {
        colorVec = black;
    }

    color = vec4(colorVec, 1.0);
}
