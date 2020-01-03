// VnmDescriptorPool.cpp

#include "VnmDescriptorPool.h"

namespace Vnm
{
    const uint32_t defaultUniformBufferDescriptorCount = 1024;
    const uint32_t defaultSampledImageDescriptorCount  = 1024;
    const uint32_t defaultSamplerDescriptorCount       = 1024;

    // TODO: Size
    void DescriptorPool::Create(Device& device)
    {
        VkDescriptorPoolSize descriptorPoolSize[3] = {};
        descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSize[0].descriptorCount = defaultUniformBufferDescriptorCount;
        descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        descriptorPoolSize[1].descriptorCount = defaultSampledImageDescriptorCount;
        descriptorPoolSize[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;
        descriptorPoolSize[2].descriptorCount = defaultSamplerDescriptorCount;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = sizeof(descriptorPoolSize) / sizeof(descriptorPoolSize[0]);
        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSize;
        descriptorPoolCreateInfo.maxSets = 16;

        vkCreateDescriptorPool(device.GetDevice(), &descriptorPoolCreateInfo, nullptr, &mDescriptorPool);
    }

    void DescriptorPool::Destroy(Device& device)
    {
        vkDestroyDescriptorPool(device.GetDevice(), mDescriptorPool, nullptr);
    }
}
