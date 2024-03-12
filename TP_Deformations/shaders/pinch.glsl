#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
    vec4 u_CameraPosition;
};

layout (std140, binding = 1) uniform Object
{
    mat4 u_Model;
};

layout (std140, binding = 2) uniform Material
{
    vec4 u_AlbedoAndShininess;
};

uniform float u_MinT;
uniform float u_MaxT;

struct VertexOutput
{
    vec3 Position;
    vec3 Normal;
    vec4 AlbedoSpecular;
};

layout(location = 0) out VertexOutput Output;

float pinch(float t,float tMin,float tMax);
float pinchDerivative(float t,float tMin,float tMax);
mat3 pinchXJacobian(float pinch_factor, float pinch_derivative);
mat3 scaleYZ(float value);

void main()
{
    vec3 normal = transpose(inverse(mat3(u_Model))) * a_Normal;

    float pinchFactor = pinch(a_Position.x, u_MinT, u_MaxT);

    vec3 position = scaleYZ(pinchFactor) * a_Position;

    float pinchDerivative = pinchDerivative(position.x, u_MinT, u_MaxT);
    mat3 pinchJacobian = pinchXJacobian(pinchFactor, pinchDerivative);
    normal = transpose(inverse(pinchJacobian)) * normal;

    vec4 worldPosition = u_Model * vec4(position, 1.0);

    gl_Position = u_ViewProjection * worldPosition;

    Output.Position = worldPosition.xyz;
    Output.Normal = normalize(normal);
    Output.AlbedoSpecular = u_AlbedoAndShininess;
}

float pinch(float t,float tMin,float tMax){
    if(t<tMin)
    return 1;

    if(t>tMin&&t<tMax)
    return 1-(.5*((t-tMin)/(tMax-tMin)));

    return.5;
}

float pinchDerivative(float t,float tMin,float tMax){
    if(t<tMin)
    return 0;

    if(t>tMin&&t<tMax)
    return -.5 / (tMax - tMin);

    return 0;
}

mat3 pinchXJacobian(float pinch_factor, float pinch_derivative)
{
    return mat3(
        1.0, 0.0, 0.0,
        pinch_derivative, pinch_factor, 0.0,
        pinch_derivative, 0.0, pinch_factor
    );
}

mat3 scaleYZ(float value)
{
    return mat3(
        1.0, 0.0, 0.0,
        0.0, value, 0.0,
        0.0, 0.0, value
    );
}

#type fragment
#version 450 core

struct VertexOutput
{
    vec3 Position;
    vec3 Normal;
    vec4 AlbedoSpecular;
};

layout(location = 0) in VertexOutput Input;

layout(location = 0) out vec4 o_Position;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_AlbedoSpecular;

void main()
{
    o_Position = vec4(Input.Position, 1.0);
    o_Normal = vec4(Input.Normal, 1.0);
    o_AlbedoSpecular = Input.AlbedoSpecular;
}
