#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerHdr;
//layout (binding = 1) uniform sampler2D samplerAlphaHdr;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;


void main()
{
    vec4 hdrFrag  = texture(samplerHdr,inUV);

    float brightness = dot(hdrFrag.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.5)
        outColor = hdrFrag*.5;
    else
        outColor = vec4(0.0,0.0,0.0,1.0);
}

