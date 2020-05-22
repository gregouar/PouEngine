#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(push_constant) uniform PER_OBJECT
{
    float radius;
}pc;


//layout (constant_id = 0) const float radius     = 0;
layout (constant_id = 0) const bool  vertical   = false;

layout (binding = 0) uniform sampler2D srcSampler1;
layout (binding = 1) uniform sampler2D srcSampler2;

layout (location = 0) in vec2 inUV;

layout (location = 0) out float outColor1;
layout (location = 1) out float outColor2;

float blur(sampler2D srcSampler)
{
    vec2 offset = vec2(1-int(vertical), int(vertical)) * pc.radius;

    return     (texture(srcSampler, inUV/*gl_FragCoord.xy*/ + offset * 1.0).r	* 0.06 +
                texture(srcSampler, inUV/*gl_FragCoord.xy*/ + offset * 0.75).r	* 0.09 +
                texture(srcSampler, inUV/*gl_FragCoord.xy*/ + offset * 0.5).r	* 0.12 +
                texture(srcSampler, inUV/*gl_FragCoord.xy*/ + offset * 0.25).r	* 0.15 +
                texture(srcSampler, inUV/*gl_FragCoord.xy*/).r	* 0.16 +
                texture(srcSampler, inUV/*gl_FragCoord.xy*/ - offset * 1.0).r 	* 0.06 +
                texture(srcSampler, inUV/*gl_FragCoord.xy*/ - offset * 0.75).r	* 0.09 +
                texture(srcSampler, inUV/*gl_FragCoord.xy*/ - offset * 0.5).r	* 0.12 +
                texture(srcSampler, inUV/*gl_FragCoord.xy*/ - offset * 0.25).r	* 0.15 );
}

void main()
{

    outColor1 = blur(srcSampler1);
    outColor1 = blur(srcSampler2);
}

