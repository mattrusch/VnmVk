// VnmSampler.h

#pragma once

#include "VnmDevice.h"

namespace Vnm
{
    class Sampler
    {
    public:
        Sampler() = default;
        ~Sampler() = default;

        void Create(Device& device);
        void Destroy(Device& device);

        VkSampler GetSampler() const { return mSampler; }
        VkSampler* GetSamplerPtr()   { return &mSampler; }

    private:
        VkSampler mSampler;
    };
}
