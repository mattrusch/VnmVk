// VnmPipeline.cpp

#include "VnmPipeline.h"
#include <type_traits>

namespace Vnm
{
    void Pipeline::Create(Device& device, VkRenderPass renderPass, VkPipelineLayout layout, VkShaderModule vertexShader, VkShaderModule fragmentShader)
    {
        VkVertexInputBindingDescription vertexInputBindingDescription;
        vertexInputBindingDescription.binding = 0;
        vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBindingDescription.stride = sizeof(float) * 3 + sizeof(float) * 2;

        VkVertexInputAttributeDescription vertexInputAttributeDescription[2] = {};
        vertexInputAttributeDescription[0].binding = vertexInputBindingDescription.binding;
        vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputAttributeDescription[0].location = 0;
        vertexInputAttributeDescription[0].offset = 0;
        vertexInputAttributeDescription[1].binding = vertexInputBindingDescription.binding;
        vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
        vertexInputAttributeDescription[1].location = 1;
        vertexInputAttributeDescription[1].offset = sizeof(float) * 3;

        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
        pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = std::extent<decltype(vertexInputAttributeDescription)>::value;
        pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;
        pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;

        VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
        pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkDynamicState dynamicStates[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
        pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineDynamicStateCreateInfo.dynamicStateCount = 2;
        pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates;

        VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
        pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pipelineViewportStateCreateInfo.viewportCount = 1;
        pipelineViewportStateCreateInfo.scissorCount = 1;

        VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
        pipelineColorBlendAttachmentState.colorWriteMask = 0xf;
        pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
        pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipelineColorBlendStateCreateInfo.attachmentCount = 1;
        pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

        VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
        pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
        pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

        VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
        pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
        pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
        pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        pipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
        pipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
        pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
        pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
        pipelineDepthStencilStateCreateInfo.front = pipelineDepthStencilStateCreateInfo.back;

        VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
        pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfos[2] = {};
        pipelineShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStageCreateInfos[0].module = vertexShader;
        pipelineShaderStageCreateInfos[0].pName = "main";
        pipelineShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        pipelineShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStageCreateInfos[1].module = fragmentShader;
        pipelineShaderStageCreateInfos[1].pName = "main";
        pipelineShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.layout = layout;
        graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
        graphicsPipelineCreateInfo.renderPass = renderPass;
        graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
        graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfos;
        graphicsPipelineCreateInfo.stageCount = 2;

        vkCreateGraphicsPipelines(device.GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &mPipeline);
    }

    void Pipeline::Destroy(Device& device)
    {
        vkDestroyPipeline(device.GetDevice(), mPipeline, nullptr);
    }
}
