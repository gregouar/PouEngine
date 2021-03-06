#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform ViewUBO {
    ///mat4 view;
    ///mat4 viewInv;
    vec2 screenOffset;
    vec2 screenSizeFactor;
    vec2 depthOffsetAndFactor;
    float proj;
} viewUbo;

layout(binding = 0, set = 1) uniform sampler samp;
layout(binding = 1, set = 1) uniform texture2DArray textures[128];

/*layout(push_constant) uniform PER_OBJECT
{
    vec4 camPosAndZoom;
}pc;*/

layout(location = 0) flat in vec4 fragColor;
layout(location = 1)      in vec2 fragTexCoord;
layout(location = 2) flat in uvec2 fragAlbedoTexId;
//layout(location = 2) in vec4 screenPosAndHeight;

layout(location = 0) out vec2 squaredDepth;

void main()
{
    float albedoA = texture(sampler2DArray(textures[fragAlbedoTexId.x], samp),
                            vec3(fragTexCoord,fragAlbedoTexId.y)).a * fragColor.a;

    if(albedoA < .01f)
        discard;

    squaredDepth.r = (1-gl_FragCoord.z);
    squaredDepth.g = (1-gl_FragCoord.z) * (1-gl_FragCoord.z);

    float dx = dFdx(1-gl_FragCoord.z);
    float dy = dFdy(1-gl_FragCoord.z);
    squaredDepth.g += 0.25*(dx*dx+dy*dy);
}




/**void main()
{
    vec4 heightPixel = texture(sampler2DArray(textures[fragTexId.x], samp),
                               vec3(fragTexCoord,fragTexId.y));

    if(heightPixel.a < 0.1)
        discard;

    float height = (heightPixel.r + heightPixel.g + heightPixel.b) * 0.33333333;
    float fragHeight = screenPosAndHeight.z + height * screenPosAndHeight.w;

    gl_FragDepth = (viewUbo.depthOffsetAndFactor.x + fragHeight * viewUbo.depthOffsetAndFactor.y) /**heightPixel.a*///;

    /*outAlbedo = fragColor * texture(sampler2DArray(textures[fragAlbedoTexId.x], samp),
                                    vec3(fragTexCoord,fragAlbedoTexId.y));


    vec4 heightPixel = texture(sampler2DArray(textures[fragHeightTexId.x], samp),
                               vec3(fragTexCoord,fragHeightTexId.y));

    float height = (heightPixel.r + heightPixel.g + heightPixel.b) * 0.33333333;
    float fragHeight = screenPosAndHeight.z + height * screenPosAndHeight.w;

    vec2 fragWorldPos = screenPosAndHeight.xy;
    fragWorldPos.y -= (fragHeight - pc.camPosAndZoom.z) * viewUbo.view[2][1];
    fragWorldPos = vec4(viewUbo.viewInv*vec4(fragWorldPos.xy,0.0,1.0)).xy;
    fragWorldPos += pc.camPosAndZoom.xy;

    gl_FragDepth = viewUbo.depthOffsetAndFactor.x + fragHeight * viewUbo.depthOffsetAndFactor.y;

    outPosition = vec4(fragWorldPos.xy, fragHeight, 0.0);

    vec3 normal = vec3(0.5,0.5,1.0);
    if(!(fragNormalTexId.x == 0 && fragNormalTexId.y == 0))
        normal = texture(sampler2DArray(textures[fragNormalTexId.x], samp), vec3(fragTexCoord,fragNormalTexId.y)).xyz;
    normal = 2.0*normal - vec3(1.0);

    normal = vec4(vec4(normal,0.0)*viewUbo.view).xyz;
    outNormal = vec4(normal,0.0);

    outRme = vec4(texture(sampler2DArray(textures[fragRmeTexId.x], samp), vec3(fragTexCoord,fragRmeTexId.y)).xyz  * fragRme, 1.0);

    if(outAlbedo.a < .99f)
        discard;*/
//} **/

