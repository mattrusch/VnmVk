// VnmDescriptorSet.h

#pragma once

#include "VnmDevice.h"
#include "VnmDescriptorSetLayout.h"
#include "VnmDescriptorPool.h"
#include "VnmBuffer.h"
#include "VnmImage.h"
#include "VnmSampler.h"

namespace Vnm
{
    class DescriptorSet
    {
    public:
        DescriptorSet() = default;
        ~DescriptorSet() = default;

        void Create(
            Device& device, 
            DescriptorPool& descriptorPool, 
            DescriptorSetLayout& descriptorSetLayout);

        void Update(
            Device& device, 
            Buffer& buffer, 
            Image& image, 
            Sampler& sampler);

        void Destroy(
            Device& device,
            DescriptorPool& descriptorPool);

        VkDescriptorSet* GetDescriptorSetPtr() { return &mDescriptorSet; }

    private:
        VkDescriptorSet mDescriptorSet;
    };
}
