// VnmDescriptorSet.cpp

#include "VnmDescriptorSet.h"
#include <cassert>

namespace
{
    const uint32_t descriptorSetCount = 1;
}

namespace Vnm
{
    void DescriptorSet::Create(Device& device, DescriptorPool& descriptorPool, DescriptorSetLayout& descriptorSetLayout)
    {
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool.GetDescriptorPool();
        descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayout.GetDescriptorSetLayoutPtr();

        vkAllocateDescriptorSets(device.GetDevice(), &descriptorSetAllocateInfo, &mDescriptorSet);
    }

    void DescriptorSet::Update(Device& device, Buffer& buffer, Image& image, Sampler& sampler)
    {
        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer = buffer.GetBuffer();
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = VK_WHOLE_SIZE;

        VkDescriptorImageInfo descriptorImageInfo = {};
        descriptorImageInfo.imageLayout = image.GetImageLayout();
        descriptorImageInfo.imageView = image.GetImageView();
        descriptorImageInfo.sampler = sampler.GetSampler();

        VkWriteDescriptorSet writeDescriptorSet[2] = {};
        writeDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet[0].dstSet = mDescriptorSet;
        writeDescriptorSet[0].descriptorCount = 1;
        writeDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet[0].dstArrayElement = 0;
        writeDescriptorSet[0].dstBinding = 0;
        writeDescriptorSet[0].pBufferInfo = &descriptorBufferInfo;
        writeDescriptorSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet[1].dstSet = mDescriptorSet;
        writeDescriptorSet[1].descriptorCount = 1;
        writeDescriptorSet[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writeDescriptorSet[1].dstArrayElement = 0;
        writeDescriptorSet[1].dstBinding = 1;
        writeDescriptorSet[1].pImageInfo = &descriptorImageInfo;

        uint32_t numWriteDescriptorSets = sizeof(writeDescriptorSet) / sizeof(writeDescriptorSet[0]);
        vkUpdateDescriptorSets(device.GetDevice(), numWriteDescriptorSets, writeDescriptorSet, 0, nullptr);
    }

    void DescriptorSet::Destroy(Device& device, DescriptorPool& descriptorPool)
    {
        vkFreeDescriptorSets(device.GetDevice(), descriptorPool.GetDescriptorPool(), descriptorSetCount, &mDescriptorSet);
    }
}
