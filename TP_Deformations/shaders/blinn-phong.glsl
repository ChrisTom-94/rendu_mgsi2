#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

struct VertexOutput
{
    vec2 TexCoord;
};

layout(location = 0) out VertexOutput Output;

void main()
{
    Output.TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
    vec4 u_CameraPosition;
};

layout(std140, binding = 3) uniform Light
{
    vec4 u_Ambient;
    vec4 u_LightPosition;
    vec4 u_LightColorAndIntensity;
};

layout(binding = 0) uniform sampler2D u_TexPositions;
layout(binding = 1) uniform sampler2D u_TexNormals;
layout(binding = 2) uniform sampler2D u_TexAlbedoSpecular;

layout(location = 0) out vec4 o_Color;

void main()
{
    vec3 position = texture(u_TexPositions, Input.TexCoord).rgb;
    vec3 normal = texture(u_TexNormals, Input.TexCoord).rgb;
    vec4 albedoSpecular = texture(u_TexAlbedoSpecular, Input.TexCoord);

    vec3 lightDirection = normalize(u_LightPosition.xyz - position);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * u_LightColorAndIntensity.rgb * u_LightColorAndIntensity.a;

    vec3 viewDirection = normalize(u_CameraPosition.xyz - position);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfwayDirection), 0.0), 32.0);
    vec3 specular = spec * vec3(1.0);

    float attenuation = 1.0 / length(u_LightPosition.xyz - position);
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 color = (u_Ambient.rgb + diffuse + specular) * albedoSpecular.rgb;

    o_Color = vec4(color, 1.0);
}
