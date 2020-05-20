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

layout(push_constant) uniform PER_OBJECT
{
    vec4 camPosAndZoom;
    vec2 shadowShift;
    vec2 lightXYonZ;
}pc;

layout(location = 0)  in mat4  inModel; //Use 4
//1
//2
//3

layout(location = 4) in vec4 inColor;
layout(location = 5) in vec3 inRme;
layout(location = 6) in vec2 inTexCoord;
layout(location = 7) in vec2 inTexExtent;
layout(location = 8) in uvec2 inTexRes;
layout(location = 9) in uvec2 inAlbedoTexId;
layout(location = 10) in uvec2 inNormalTexId;
layout(location = 11) in uvec2 inRmeTexId;
/*layout(location = 9) in uvec2 inHeightTexId;*/
/*layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inSize;
layout(location = 2) in vec2 inCenter;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec2 inTexExtent;
layout(location = 5) in uvec2 inTexId;*/

layout(location = 0) flat out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uvec2 fragAlbedoTexId;
//layout(location = 2) out vec4 screenPosAndHeight;


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
    vec4 localPos = vec4(vertPos[gl_VertexIndex],0,1.0);
    vec4 worldPos = inModel * localPos;
    worldPos = vec4(worldPos.xyz/worldPos.w, 1.0);

    vec4 projWorldPos = vec4(worldPos.xy - worldPos.z*pc.lightXYonZ,
                             0.0,1.0);

    gl_Position = viewUbo.view*(projWorldPos - vec4(pc.camPosAndZoom.xyz,0.0));
    gl_Position.z = worldPos.z;

    gl_Position.xy -= pc.shadowShift * 0.5;
    vec2 shadowSizeFactor = 2.0/(2.0/viewUbo.screenSizeFactor+abs(pc.shadowShift));

    gl_Position.xyz = gl_Position.xyz * vec3(shadowSizeFactor, viewUbo.depthOffsetAndFactor.y)
                        + vec3(viewUbo.screenOffset, viewUbo.depthOffsetAndFactor.x);

    fragColor    = inColor;
    fragTexCoord = inTexExtent * localPos.xy + inTexCoord;;
    fragAlbedoTexId = inAlbedoTexId;
}

/*void main()
{
    vec4 spriteViewCenter = viewUbo.view*vec4(vec3(inPos.xy,0.0)-pc.camPosAndZoom.xyz,1.0);
    //spriteViewCenter.xy -= min(pc.shadowShift, 0.0);
    spriteViewCenter.xy -= pc.shadowShift * 0.5;
    spriteViewCenter.z = inPos.z;

    gl_Position = vec4(spriteViewCenter.xyz + vec3((vertPos[gl_VertexIndex] * inSize.xy - inCenter), 0.0), 1.0);

    screenPosAndHeight = vec4(gl_Position.xyz, inSize.z);

    vec2 shadowSizeFactor = 2.0/(2.0/viewUbo.screenSizeFactor+abs(pc.shadowShift));

    gl_Position.xyz = gl_Position.xyz * vec3(shadowSizeFactor, viewUbo.depthOffsetAndFactor.y)
                        + vec3(viewUbo.screenOffset, viewUbo.depthOffsetAndFactor.x);

    fragTexCoord = inTexExtent * vertPos[gl_VertexIndex] + inTexCoord;
    fragTexId    = inTexId;
}*/


