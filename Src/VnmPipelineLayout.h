// VnmPipelineLayout.h

#pragma once

#include "VnmDevice.h"
#include "VnmDescriptorSetLayout.h"

namespace Vnm
{
    class PipelineLayout
    {
    public:
        PipelineLayout() = default;
        ~PipelineLayout() = default;

        void Create(Device& device);
        void Create(Device& device, DescriptorSetLayout& descriptorSetLayout);

        VkPipelineLayout GetPipelineLayout() const { return mPipelineLayout; }

    private:
        VkPipelineLayout mPipelineLayout;
    };
}
