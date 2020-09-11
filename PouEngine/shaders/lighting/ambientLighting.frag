#version 450
#extension GL_ARB_separate_shader_objects : enable

/*layout (constant_id = 0) const int envMapMipsCount = 1;
layout (constant_id = 1) const float const_aoIntensity = 1;
layout (constant_id = 2) const float const_gioIntensity = 1;*/


layout (set = 0, binding = 0) uniform sampler2D samplerAlbedo;
layout (set = 0, binding = 1) uniform sampler2D samplerPosition;
layout (set = 0, binding = 2) uniform sampler2D samplerNormal;
layout (set = 0, binding = 3) uniform sampler2D samplerRme;
layout (set = 0, binding = 4) uniform sampler2D samplerBentNormals;

/*layout (set = 0, binding = 4) uniform sampler2D samplerAlphaAlbedo;
layout (set = 0, binding = 5) uniform sampler2D samplerAlphaPosition;
layout (set = 0, binding = 6) uniform sampler2D samplerAlphaNormal;
layout (set = 0, binding = 7) uniform sampler2D samplerAlphaRme;

layout (set = 0, binding = 8) uniform sampler2D samplerBentNormals;
layout (set = 0, binding = 9) uniform sampler2D samplerGlobalIllumination;*/

layout (set = 0, binding = 5) uniform texture2D brdflut;

/*layout(set = 1, binding = 0) uniform sampler samp;
layout(set = 1, binding = 1) uniform texture2DArray textures[128];*/

layout(set = 1, binding = 0) uniform sampler    samp;
/*layout(set = 1, binding = 1) uniform texture2D  texEnvMap;*/
layout(set = 1, binding = 1) uniform AmbientLightingUbo {
    vec4 ambientLight;
    //bool enableEnvMap;
} ubo;

layout(push_constant) uniform PER_OBJECT
{
    vec4 camPosAndZoom;
}pc;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;
//layout (location = 1) out vec4 outAlphaColor;

/*vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.x, v.y), -asin(v.z));
    uv *= vec2(0.15915494309, 0.31830988618);
    uv += 0.5;
    return uv;
}

//This should be replace by something better
vec3 hash(vec3 a)
{
   a = fract(a*0.8);
   a += dot(a, a.xyz + 19.19);
   return fract((a.xxy + a .yxx) * a.zyx);
}


vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec4 computeAmbientLighting(vec4 fragAlbedo, vec3 fragPos, vec4 fragNormal, vec4 fragBentNormal, vec3 fragRme, vec3 globalIll)
{

    vec3 ambientLighting = ubo.ambientLight.rgb * ubo.ambientLight.a;

    //Change this for better random (use pregenerated)
    vec3 rVec = hash(fragPos);
    vec3 viewDirection = normalize(pc.camPosAndZoom.xyz - fragPos);
    float NdotV = max(dot(fragNormal.xyz, viewDirection), 0.0);

    //vec3 ortho_viewDirection = vec3(<<cos(45*PI/180)*cos(30*PI/180)<<,
    //                                <<sin(45*PI/180)*cos(30*PI/180)<<,
    //                                <<sin(30*PI/180)<<);

    vec3 surfaceReflection0 = vec3(0.04);
    surfaceReflection0 = mix(surfaceReflection0, fragAlbedo.rgb, fragRme.g);

    vec3 F = fresnelSchlickRoughness(NdotV, surfaceReflection0, fragRme.r);
    vec3 kS = F;
    vec3 kD = (1.0 - F)*(1.0 - fragRme.g);
    //kD *= pow(fragBentNormal.a,2.0);
    vec3 irradiance = ambientLighting;
    float occlusion = max(min(pow(fragBentNormal.a,const_aoIntensity), pow(1.0-abs(fragBentNormal.z),const_gioIntensity)), fragNormal.w); //We dont want to occlude truly transparent fragments

    vec3 reflectionView = reflect(-viewDirection, fragNormal.xyz);
    //reflectionView += mix(vec3(0.0),rVec,fragRme.r*.25);
    vec3 reflectionColor = ambientLighting;
    vec2 envUV = vec2(0,0);

    if(ubo.enableEnvMap == true)
    {
        envUV = sampleSphericalMap(normalize(reflectionView));
        reflectionColor = textureLod(sampler2D(texEnvMap,samp), vec2(envUV), fragRme.r * envMapMipsCount).rgb;
    }

    vec2 envBRDF  = texture(sampler2D(brdflut,samp), vec2(NdotV, fragRme.r)).rg;
    vec3 specular = reflectionColor  * (F * envBRDF.x + envBRDF.y);

    return vec4(fragAlbedo.rgb * irradiance * occlusion * kD + specular + globalIll*(1.0-fragNormal.w), fragAlbedo.a);
}*/

float discretize(float v, uint steps)
{
    return floor(v * steps)/steps;
}

vec3 discretize(vec3 v, uint steps)
{
    return floor(v * steps)/steps;
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec4 computeAmbientLighting(vec4 fragAlbedo, vec3 fragPos, vec4 fragNormal, vec3 fragRme, float fragAO)
{
    if(fragRme.z > 0)
        return vec4(fragRme.z * 50.0 * fragAlbedo.rgb,fragAlbedo.a);

    vec3 ambientLighting = ubo.ambientLight.rgb * ubo.ambientLight.a;

    vec3 viewDirection = normalize(vec3(0.0,0.0,1000.0) - fragPos);
    float NdotV = max(dot(fragNormal.xyz, viewDirection), 0.0);

    vec3 surfaceReflection0 = vec3(0.04);
    surfaceReflection0 = mix(surfaceReflection0, fragAlbedo.rgb, fragRme.g);

    vec3 F = fresnelSchlickRoughness(NdotV, surfaceReflection0, fragRme.r);
    vec3 kS = F;
    vec3 kD = (1.0 - F)*(1.0 - fragRme.g);
    vec3 irradiance = ambientLighting;
    vec3 reflectionView = reflect(-viewDirection, fragNormal.xyz);

    vec3 reflectionColor = ambientLighting  /* * (1+ fragRme.g) */;

    reflectionView = normalize(reflectionView);
   /* vec3 pickAlbedo = texture(samplerAlbedo, inUV+ reflectionView.xy*.01).rgb;
    reflectionColor = mix(pickAlbedo * ambientLighting, ambientLighting, .0) ;*/


    vec2 envBRDF  = texture(sampler2D(brdflut,samp), vec2(NdotV, fragRme.r)).rg;
    vec3 specular = reflectionColor  * (F * envBRDF.x + envBRDF.y);

    //specular = discretize(specular,3);

    vec4 lighting = vec4(fragAlbedo.rgb * irradiance * kD + specular, fragAlbedo.a);
    lighting.rgb = mix(fragAlbedo.rgb*ambientLighting.rgb, lighting.rgb, .25+fragRme.g*.75);
    //lighting += vec4(fragRme.z * 50.0 * fragAlbedo.rgb,0.0);

    lighting.rgb *= fragAO;

    return lighting;

}

/**vec4 computeAmbientLighting(vec4 fragAlbedo, vec3 fragPos, vec4 fragNormal, vec3 fragRme)
{
    vec4 lighting = vec4(fragAlbedo.rgb*ubo.ambientLight.rgb*ubo.ambientLight.a,fragAlbedo.a);
    lighting += vec4(fragRme.z * 50.0 * fragAlbedo.rgb,0.0);
    return lighting;
}**/

void main()
{
    vec4 fragAlbedo = texture(samplerAlbedo, inUV);
    vec3 fragPos    = texture(samplerPosition, inUV).xyz;
    vec4 fragNormal = texture(samplerNormal, inUV);
    vec3 fragRme    = texture(samplerRme, inUV).xyz;
    float fragAO = texture(samplerBentNormals, inUV).w;
    /*vec3 globalIll = texture(samplerGlobalIllumination, inUV).rgb;

    outColor = computeAmbientLighting(fragAlbedo, fragPos, fragNormal, fragBentNormal, fragRme, globalIll);
	outColor.rgb = pow(outColor.rgb, vec3(2.2));


    fragAlbedo = texture(samplerAlphaAlbedo, inUV);
    fragPos    = texture(samplerAlphaPosition, inUV).xyz;
    fragNormal = texture(samplerAlphaNormal, inUV);
    //fragNormal.xyz  = normalize(fragNormal.xyz);//This should be already normalized heh
    fragRme    = texture(samplerAlphaRme, inUV).xyz;

    outAlphaColor = computeAmbientLighting(fragAlbedo, fragPos, fragNormal, fragBentNormal, fragRme, globalIll);
	outAlphaColor.rgb = pow(outAlphaColor.rgb, vec3(2.2));*/

	fragAlbedo.rgb = pow(fragAlbedo.rgb, vec3(2.2));
	outColor = computeAmbientLighting(fragAlbedo, fragPos, fragNormal, fragRme, fragAO);
	//outColor.rgb = pow(outColor.rgb, vec3(2.2));
}

