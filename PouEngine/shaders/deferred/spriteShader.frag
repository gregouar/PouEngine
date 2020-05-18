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

layout(push_constant) uniform PER_OBJECT
{
    vec4 camPosAndZoom;
}pc;

layout(location = 0) flat in vec4 fragColor;
//layout(location = 1) flat in vec3 fragRmt;
layout(location = 1)      in mat3 fragTBN; //Use 3
//2
//3
layout(location = 4)      in vec2 fragTexCoord;
layout(location = 5) flat in uvec2 fragAlbedoTexId;
layout(location = 6) flat in uvec2 fragNormalTexId;
/*layout(location = 4) flat in uvec2 fragHeightTexId;
layout(location = 6) flat in uvec2 fragRmtTexId;*/
layout(location = 7) flat in uvec2 inTexRes;
layout(location = 8) in vec4 originWorldPos;
layout(location = 9) in vec4 xBasisVect;
layout(location = 10) in vec4 yBasisVect;
layout(location = 11) in vec4 localPos;
//layout(location = 4) in vec3 screenPos;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outPosition;
layout(location = 2) out vec4 outNormal;
/*layout(location = 3) out vec4 outRmt;*/



float discretize(float v, uint steps)
{
    return floor(v * steps)/steps;
}

void main()
{
    outAlbedo = fragColor * texture(sampler2DArray(textures[fragAlbedoTexId.x], samp),
                                    vec3(fragTexCoord,fragAlbedoTexId.y));

    if(outAlbedo.a == 0.0)
        discard;

    outPosition = originWorldPos + xBasisVect * discretize(localPos.x, inTexRes.x) + yBasisVect * discretize(localPos.y, inTexRes.y);
    //outPosition = originWorldPos + xBasisVect * localPos.x + yBasisVect * localPos.y;

    //outPosition = vec4(relWorldPos.xyz/relWorldPos.w, 0.0);

    //gl_FragDepth = viewUbo.depthOffsetAndFactor.x + /*screenPos.z*/ gl_FragCoord.z * viewUbo.depthOffsetAndFactor.y;



    vec3 normal = vec3(0.5,0.5,1.0);
	if(!(fragNormalTexId.x == 0 && fragNormalTexId.y == 0))
        normal = texture(sampler2DArray(textures[fragNormalTexId.x], samp), vec3(fragTexCoord,fragNormalTexId.y)).xyz;
    normal = 2.0*normal - vec3(1.0);
    normal = fragTBN*normal;

    //normal = vec4(vec4(normal,0.0)*viewUbo.view).xyz;
    outNormal = vec4(normalize(normal),outAlbedo.a);

}

