// SimpleFrag.fs

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 inUv;
layout (location = 1) in vec4 inColor;
layout (location = 2) in float inFog;
layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform texture2D colorTexture;
layout (binding = 2) uniform sampler colorSampler;

void main()
{
	vec4 color = texture(sampler2D(colorTexture, colorSampler), inUv) * inColor;
    vec4 fogColor = vec4(0.65, 0.75, 1.0, 1.0);
    outColor = mix(color, fogColor, inFog);
}
