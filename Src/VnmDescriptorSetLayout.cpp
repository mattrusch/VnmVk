// VnmDescriptorSetLayout.cpp

#include "VnmDescriptorSetLayout.h"

namespace Vnm
{
    void DescriptorSetLayout::Create(Device& device, const VkDescriptorSetLayoutBinding* pBindings, uint32_t bindingCount)
    {
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = bindingCount;
        layoutCreateInfo.pBindings = pBindings;

        vkCreateDescriptorSetLayout(device.GetDevice(), &layoutCreateInfo, nullptr, &mDescriptorSetLayout);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;
        pipelineLayoutCreateInfo.setLayoutCount = 1;

        vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
    }

    void DescriptorSetLayout::Destroy(Device& device)
    {
        vkDestroyPipelineLayout(device.GetDevice(), mPipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(device.GetDevice(), mDescriptorSetLayout, nullptr);
    }
}
