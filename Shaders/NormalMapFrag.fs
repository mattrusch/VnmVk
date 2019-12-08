// NormalMap.fs

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 inUv;
layout (location = 1) in vec3 inTangent;
layout (location = 2) in vec3 inBitangent;
layout (location = 3) in vec3 inNormal;

layout (binding = 1) uniform texture2D colorTexture;
layout (binding = 2) uniform sampler colorSampler;
layout (binding = 3) uniform texture2D normalTexture;
layout (binding = 4) uniform sampler normalSampler;

layout (location = 0) out vec4 outColor;

void main()
{
    vec4 normalSample = texture(sampler2D(normalTexture, normalSampler), inUv);
	vec4 colorSample = texture(sampler2D(colorTexture, colorSampler), inUv);

    outColor = vec4(1.0);
    outColor.rgb = normalSample.rgb;
}
