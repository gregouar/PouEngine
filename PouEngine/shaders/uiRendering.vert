#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform ViewUBO {
    mat4 view;
    mat4 viewInv;
    vec2 screenOffset;
    vec2 screenSizeFactor;
    vec2 depthOffsetAndFactor;
    float proj;
} viewUbo;


layout(location = 0)  in mat4  inModel; //Use 4
//1
//2
//3

layout(location = 4) in vec4 inColor;
layout(location = 5) in vec2 inTexCoord;
layout(location = 6) in vec2 inTexExtent;
layout(location = 7) in uvec2 inAlbedoTexId;

layout(location = 0) flat out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uvec2 fragAlbedoTexId;

vec2 vertPos[4] = vec2[](
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
);

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
    gl_Position = viewUbo.view * (inModel * vec4(vertPos[gl_VertexIndex],0.0,1.0));
    gl_Position = vec4(gl_Position.xyz/gl_Position.w, 1.0);

    gl_Position.xyz = gl_Position.xyz * vec3(viewUbo.screenSizeFactor, viewUbo.depthOffsetAndFactor.y)
                        + vec3(viewUbo.screenOffset, viewUbo.depthOffsetAndFactor.x);

    fragTexCoord = inTexExtent * vertPos[gl_VertexIndex] + inTexCoord;
    fragColor    = inColor;
    fragAlbedoTexId = inAlbedoTexId;
}


