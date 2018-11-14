// SimpleVert.vs

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform PerObjectUbo
{
	mat4 wvp;
} ubo;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUv;
layout (location = 0) out vec2 outUv;
layout (location = 1) out vec4 outColor;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.wvp * vec4(inPos, 1);
	outUv = inUv;
	outColor = inUv.xyxy * 0.5 + 0.5;
}