// VnmDescriptorSet.cpp

#include "VnmDescriptorSet.h"
#include <vector>
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

    void DescriptorSet::Update(Device& device, std::vector<VkDescriptorType>& descriptorTypes, std::vector<Buffer*>& buffers, std::vector<Image*> images, std::vector<Sampler*> samplers)
    {
        assert(descriptorTypes.size() == buffers.size() + images.size());
        assert(images.size() == samplers.size());

        // TODO: Bake or statically allocate the following descriptor info arrays
        std::vector<VkDescriptorBufferInfo> descriptorBufferInfos(buffers.size());
        for (size_t i = 0, size = buffers.size(); i < size; ++i)
        {
            descriptorBufferInfos[i].buffer = buffers[i]->GetBuffer();
            descriptorBufferInfos[i].offset = 0;
            descriptorBufferInfos[i].range = VK_WHOLE_SIZE;
        }

        std::vector<VkDescriptorImageInfo> descriptorImageInfos(images.size());
        for (size_t i = 0, size = images.size(); i < size; ++i)
        {
            descriptorImageInfos[i].imageLayout = images[i]->GetImageLayout();
            descriptorImageInfos[i].imageView = images[i]->GetImageView();
            descriptorImageInfos[i].sampler = samplers[i]->GetSampler();
        }
        
        int curBufferIndex = 0;
        int curImageIndex = 0;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets(descriptorTypes.size());
        for (size_t i = 0, size = descriptorTypes.size(); i < size; ++i)
        {
            writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[i].dstSet = mDescriptorSet;
            writeDescriptorSets[i].descriptorCount = 1;
            writeDescriptorSets[i].dstArrayElement = 0;
            writeDescriptorSets[i].dstBinding = static_cast<uint32_t>(i);
            writeDescriptorSets[i].descriptorType = descriptorTypes[i];

            switch (descriptorTypes[i])
            {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                writeDescriptorSets[i].pBufferInfo = descriptorBufferInfos.data() + curBufferIndex++;
                break;
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                writeDescriptorSets[i].pImageInfo = descriptorImageInfos.data() + curImageIndex++;
                break;
            default:
                assert(!"Unknown descriptor type");
                break;
            }
        }
    }

    void DescriptorSet::Destroy(Device& device, DescriptorPool& descriptorPool)
    {
        vkFreeDescriptorSets(device.GetDevice(), descriptorPool.GetDescriptorPool(), descriptorSetCount, &mDescriptorSet);
    }
}
