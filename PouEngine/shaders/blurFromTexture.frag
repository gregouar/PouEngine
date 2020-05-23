#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(push_constant) uniform PER_OBJECT
{
    uvec2    texId1;
    uvec2    texId2;
    float   radius;
}pc;


//layout (constant_id = 0) const float radius     = 0;
layout (constant_id = 0) const bool  vertical   = false;

layout(binding = 0, set = 0) uniform sampler samp;
layout(binding = 1, set = 0) uniform texture2DArray textures[128];
layout(set = 0, binding = 2) uniform texture2DArray renderedTextures[32];

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec2 outColor1;
//layout (location = 1) out float outColor2;

vec2 blur(uvec2 texId)
{
    vec2 offset = vec2(1-int(vertical), int(vertical)) * pc.radius;

    return     (texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/ + offset * 1.0,texId.y)).rg	* 0.06 +
                texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/ + offset * 0.75,texId.y)).rg	* 0.09 +
                texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/ + offset * 0.5,texId.y)).rg	* 0.12 +
                texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/ + offset * 0.25,texId.y)).rg	* 0.15 +
                texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/,texId.y)).rg	* 0.16 +
                texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/ - offset * 1.0,texId.y)).rg 	* 0.06 +
                texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/ - offset * 0.75,texId.y)).rg	* 0.09 +
                texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/ - offset * 0.5,texId.y)).rg	* 0.12 +
                texture(sampler2DArray(renderedTextures[texId.x], samp), vec3(inUV/*gl_FragCoord.xy*/ - offset * 0.25,texId.y)).rg	* 0.15 );
}


void main()
{
   // gl_FragDepth = blur(pc.texId1);
    outColor1 = blur(pc.texId2);
    //outColor2 = blur(pc.texId2);

}

