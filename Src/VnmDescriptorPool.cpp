// VnmDescriptorPool.cpp

#include "VnmDescriptorPool.h"

namespace Vnm
{
    void DescriptorPool::Create(Device& device)
    {
        VkDescriptorPoolSize descriptorPoolSize[2] = {};
        descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSize[0].descriptorCount = 1;
        descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        descriptorPoolSize[1].descriptorCount = 1;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 2;
        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSize;
        descriptorPoolCreateInfo.maxSets = 16;

        vkCreateDescriptorPool(device.GetDevice(), &descriptorPoolCreateInfo, nullptr, &mDescriptorPool);
    }

    void DescriptorPool::Destroy(Device& device)
    {
        vkDestroyDescriptorPool(device.GetDevice(), mDescriptorPool, nullptr);
    }
}
