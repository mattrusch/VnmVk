// VnmDescriptorLayout.h

#pragma once

#include "VnmDevice.h"

namespace Vnm
{
    class DescriptorSetLayout
    {
    public:
        DescriptorSetLayout() = default;
        ~DescriptorSetLayout() = default;

        void Create(
            Device& device, 
            const VkDescriptorSetLayoutBinding* pBindings, 
            uint32_t bindingCount);

        void Destroy(
            Device& device);

        VkDescriptorSetLayout* GetDescriptorSetLayoutPtr() { return &mDescriptorSetLayout; }

    private:
        VkDescriptorSetLayout mDescriptorSetLayout;
        VkPipelineLayout      mPipelineLayout;
    };
}
