#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
    vec3 u_CameraPosition;
};

layout(std140, binding = 1) uniform Model
{
    mat4 u_Model;
};

struct VertexOutput
{
    vec3 Position;
    vec3 Normal;
};

layout(location = 0) out VertexOutput Output;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_Position, 1.0);
    Output.Position = worldPosition.xyz;
    Output.Normal = normalize(mat3(u_Model) * a_Normal);

    gl_Position = u_ViewProjection * worldPosition;
}

#type fragment
#version 450 core

struct VertexOutput
{
    vec3 Position;
    vec3 Normal;
};

layout(location = 0) in VertexOutput Input;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
    vec3 u_CameraPosition;
};

layout(std140, binding = 2) uniform Light
{
    vec4 u_AmbientLightColor;
    vec4 u_LightPosition;
    vec4 u_LightColorAndIntensity;
};

layout(location = 0) out vec4 o_Color;

void main()
{
    vec3 lightDirection = normalize(u_LightPosition.xyz - Input.Position);
    vec3 diffuse = max(dot(Input.Normal, lightDirection), 0.0) * u_LightColorAndIntensity.rgb * u_LightColorAndIntensity.a;

    vec3 viewDirection = normalize(u_CameraPosition - Input.Position);
    vec3 halfDirection = normalize(lightDirection + viewDirection);
    vec3 specular = pow(max(dot(Input.Normal, halfDirection), 0.0), 64.0) * u_LightColorAndIntensity.rgb * u_LightColorAndIntensity.a;

    float attenuation = 1.0 / length(u_LightPosition.xyz - Input.Position);
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 seaColor = vec3(0.0, 0.51, 0.73);

    vec3 color = (u_AmbientLightColor.rgb + diffuse + specular) * seaColor;

    o_Color = vec4(color, 1.0);
}
