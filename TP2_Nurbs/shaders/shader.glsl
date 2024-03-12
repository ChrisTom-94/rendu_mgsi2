#type vertex
#version 450 core

layout(location=0)in vec2 a_Position;
layout(location=1)in vec2 a_TexCoord;

struct VertexOutput{
    vec2 TexCoord;
};

layout(location=0)out VertexOutput Output;

void main()
{
    Output.TexCoord=a_TexCoord;
    gl_Position=vec4(a_Position,0., 1.);
}

#type fragment
#version 450 core

struct VertexOutput{
    vec2 TexCoord;
};

layout(location=0)in VertexOutput Input;

layout(binding=0)uniform sampler2D u_Texture;

layout(location=0)out vec4 o_Color;

void main(){
    o_Color=texture(u_Texture,Input.TexCoord);
}
