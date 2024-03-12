#type vertex
#version 450 core

#define PI 3.14159

layout(location=0)in vec3 a_Position;
layout(location=1)in vec3 a_Normal;
layout(location=2)in vec2 a_TexCoord;

layout(std140,binding=0)uniform Camera
{
    mat4 u_ViewProjection;
    vec4 u_CameraPosition;
};

layout(std140,binding=1)uniform Object
{
    mat4 u_Model;
};

layout(std140,binding=2)uniform Material
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

layout(location=0)out VertexOutput Output;

float twist(float t,float tMin,float tMax,float maxAngle);
float twistDerivative(float t,float tMin,float tMax,float maxAngle);
mat3 twistYJacobian(vec3 point,float twist_angle,float derivative);
mat3 rotateY(float angle);

void main()
{
    vec3 normal=transpose(inverse(mat3(u_Model)))*a_Normal;

    vec3 position = a_Position;

    float min_t = u_MinT;
    float max_t = u_MaxT;

    float maxAngle = 2*PI;

    float angle=twist(position.y,min_t,max_t,maxAngle);

    position=rotateY(angle)*position;

    float derivative=twistDerivative(position.y,min_t,max_t,maxAngle);
    mat3 jacobian=twistYJacobian(position,angle,derivative);
    normal=transpose(inverse(jacobian))*normal;

    vec4 worldPosition=u_Model*vec4(position,1.);

    Output.Position=worldPosition.xyz;
    Output.Normal=normalize(normal);
    Output.AlbedoSpecular=u_AlbedoAndShininess;

    gl_Position=u_ViewProjection*worldPosition;

}

float twist(float t,float tMin,float tMax,float maxAngle)
{
    if(t<tMin)
    return 0;

    if(t>tMin&&t<tMax)
    return((t-tMin)/(tMax-tMin))*maxAngle;

    return maxAngle;
}

float twistDerivative(float t,float tMin,float tMax,float maxAngle)
{
    if(t<tMin)
    return 0;

    if(t>tMin&&t<tMax)
    return maxAngle/(tMax-tMin);

    return 0;
}

mat3 twistYJacobian(vec3 point,float twist_angle,float derivative)
{
    float cos_t=cos(twist_angle);
    float sin_t=sin(twist_angle);

    float a=derivative*(-point.x*sin_t+point.z*cos_t);
    float b=-derivative*(point.x*cos_t+point.z*sin_t);

    return mat3(
        cos_t,a,sin_t,
        .0,1.,.0,
        -sin_t,b,cos_t
    );
}

mat3 rotateY(float angle)
{
    float cos_t=cos(angle);
    float sin_t=sin(angle);

    return mat3(
        cos_t,.0,sin_t,
        .0,1.,.0,
        -sin_t,.0,cos_t
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

layout(location=0)in VertexOutput Input;

layout(location=0)out vec4 o_Position;
layout(location=1)out vec4 o_Normal;
layout(location=2)out vec4 o_AlbedoSpecular;

void main()
{
    o_Position=vec4(Input.Position, 1.0);
    o_Normal=vec4(Input.Normal, 1.0);
    o_AlbedoSpecular=Input.AlbedoSpecular;
}
