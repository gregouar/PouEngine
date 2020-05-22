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

layout(binding = 0, set = 1) uniform sampler samp;
layout(binding = 1, set = 1) uniform texture2DArray textures[128];

layout(location = 0)       in vec2  fragUV;
layout(location = 1)  flat in uvec2 fragAlbedoTexId;

layout(location = 0) out float squaredDepth;

void main()
{
    float albedoA = texture(sampler2DArray(textures[fragAlbedoTexId.x], samp),
                            vec3(fragUV,fragAlbedoTexId.y)).a;

    if(albedoA < .99f)
        discard;

    squaredDepth = (1-gl_FragCoord.z) * (1-gl_FragCoord.z);

    float dx = dFdx(1-gl_FragCoord.z);
    float dy = dFdy(1-gl_FragCoord.z);
    squaredDepth += 0.25*(dx*dx+dy*dy);
}

