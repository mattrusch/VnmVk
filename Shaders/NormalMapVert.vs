// SimpleVert.vs

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform PerObjectUbo
{
	mat4 wvp;
	mat4 world;
} ubo;

layout (binding = 5) uniform PerSceneUbo
{
    vec3 eyeWorldPos;
} sceneUbo;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inTangent;
layout (location = 3) in vec3 inUv;

layout (location = 0) out vec2 outUv;
layout (location = 1) out vec3 outTangent;
layout (location = 2) out vec3 outBitangent;
layout (location = 3) out vec3 outNormal;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.wvp * vec4(inPos, 1.0);
	outUv = inUv.xy;
	vec3 worldNormal = (ubo.world * vec4(inNormal, 0.0)).xyz;
	vec3 worldTangent = (ubo.world * vec4(inTangent.xyz, 0.0)).xyz;
    vec3 worldBitangent = cross(worldTangent, worldNormal) * inTangent.w;
}
