// VnmDescriptorPool.h

#pragma once

#include "VnmDevice.h"

namespace Vnm
{
    class DescriptorPool
    {
    public:
        DescriptorPool() = default;
        ~DescriptorPool() = default;

        void Create(Device& device);
        void Destroy(Device& device);

        VkDescriptorPool GetDescriptorPool() const { return mDescriptorPool; }

    private:
        VkDescriptorPool mDescriptorPool;
    };
}
