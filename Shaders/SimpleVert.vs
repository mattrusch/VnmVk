// SimpleVert.vs

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform PerObjectUbo
{
	mat4 wvp;
} ubo;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inUv;
layout (location = 2) in vec3 inNormal;
layout (location = 0) out vec2 outUv;
layout (location = 1) out vec4 outColor;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.wvp * vec4(inPos, 1.0);
	outUv = inUv.xy;
	outColor = vec4(dot(inNormal, normalize(vec3(1.0, 1.0, 1.0)))) * 0.5 + 0.5;
}
