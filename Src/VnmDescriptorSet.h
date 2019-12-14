// VnmDescriptorSet.h

#pragma once

#include "VnmDevice.h"
#include "VnmDescriptorSetLayout.h"
#include "VnmDescriptorPool.h"
#include "VnmBuffer.h"
#include "VnmImage.h"
#include "VnmSampler.h"

#include <vector>

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

        void Update(
            Device& device,
            std::vector<VkDescriptorType>& descriptorTypes,
            std::vector<Buffer*>& buffers,
            std::vector<Image*> images,
            std::vector<Sampler*> samplers);

        void Destroy(
            Device& device,
            DescriptorPool& descriptorPool);

        VkDescriptorSet* GetDescriptorSetPtr() { return &mDescriptorSet; }

    private:
        VkDescriptorSet mDescriptorSet;
    };
}
