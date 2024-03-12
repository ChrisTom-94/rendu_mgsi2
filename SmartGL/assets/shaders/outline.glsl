#type vertex
#version 450 core

layout(location=0)in vec3 a_Position;
layout(location=1)in vec2 a_TexCoord;

struct VertexOutput
{
    vec2 TexCoord;
};

layout(location=0)out VertexOutput Output;

void main()
{
    gl_Position=vec4(a_Position,1.);
    Output.TexCoord=a_TexCoord;
}

#type fragment
#version 450 core

struct VertexOutput
{
    vec2 TexCoord;
};

layout(location=0)in VertexOutput Input;

layout(binding=0)uniform sampler2D u_DepthTexture;
layout(binding=1)uniform sampler2D u_ColorTexture;

layout(location=0)out vec4 o_Color;

bool isEdge(mat3 pixels);
mat3 getNeighbourhood(sampler2D depthTexture,vec2 texCoord);
float linearizeDepth(float z);

mat3 sobel_x=mat3(
    1.,0.,-1.,
    2.,0.,-2.,
    1.,0.,-1.
);

mat3 sobel_y=mat3(
    1.,2.,1.,
    0.,0.,0.,
    -1.,-2.,-1.
);

void main()
{
    vec3 edgeColor=vec3(1.);

    mat3 pixels=getNeighbourhood(u_DepthTexture,Input.TexCoord);
    if(isEdge(pixels))
    {
        o_Color=vec4(edgeColor,1.);
        return;
    }

    vec3 color=texture(u_ColorTexture,Input.TexCoord).rgb;
    o_Color=vec4(color,1.);
}

bool isEdge(mat3 I)
{
    float gradient_x=dot(sobel_x[0],I[0])+dot(sobel_x[1],I[1])+dot(sobel_x[2],I[2]);
    float gradient_y=dot(sobel_y[0],I[0])+dot(sobel_y[1],I[1])+dot(sobel_y[2],I[2]);

    float depthGradient=sqrt(gradient_x*gradient_x+gradient_y*gradient_y);
    float depthThreshold=.1;
    return depthGradient>depthThreshold;
}

mat3 getNeighbourhood(sampler2D depthTexture,vec2 texCoord)
{
    mat3 neighbourhood;
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            float depth=texture(depthTexture,texCoord+vec2(i-1,j-1)).r;
            neighbourhood[i][j]=linearizeDepth(depth);
        }
    }
    return neighbourhood;
}

float linearizeDepth(float z)
{
    float n=.1;
    float f=100.;
    return(2.*n)/(f+n-z*(f-n));
}
