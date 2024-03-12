#type vertex
#version 450 core

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

struct VertexOutput
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
};

layout(location=0)out VertexOutput Output;

void main()
{
    vec4 worldPosition= u_Model * vec4(a_Position,1.);
    mat3 normalMatrix=transpose(inverse(mat3(u_Model)));

    Output.Position=worldPosition.xyz;
    Output.Normal=normalMatrix*a_Normal;
    Output.TexCoord=a_TexCoord;
    gl_Position=u_ViewProjection*worldPosition;
}

#type fragment
#version 450 core

struct VertexOutput
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
};

layout(location=0)in VertexOutput Input;

layout(std140,binding=2)uniform Material
{
    vec4 u_AlbedoSpecular;
};

layout(location=0)out vec4 o_Position;
layout(location=1)out vec4 o_Normal;
layout(location=2)out vec4 o_AlbedoSpecular;

void main()
{
    o_Position=vec4(Input.Position,1.);
    o_Normal=vec4(normalize(Input.Normal),1.);
    o_AlbedoSpecular=u_AlbedoSpecular;
}
