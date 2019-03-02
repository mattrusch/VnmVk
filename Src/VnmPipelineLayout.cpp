// VnmPipelineLayout.cpp

#include "VnmPipelineLayout.h"

namespace Vnm
{
    void PipelineLayout::Create(Device& device)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
    }

    void PipelineLayout::Create(Device& device, DescriptorSetLayout& descriptorSetLayout)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.GetDescriptorSetLayoutPtr();
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
    }

    void PipelineLayout::Destroy(Device& device)
    {
        vkDestroyPipelineLayout(device.GetDevice(), mPipelineLayout, nullptr);
    }
}
