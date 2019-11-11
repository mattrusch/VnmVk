// VnmSampler

#include "VnmSampler.h"
#include <cfloat>

namespace Vnm
{

    void Sampler::Create(Device& device)
    {
        VkSamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = FLT_MAX;

        vkCreateSampler(device.GetDevice(), &samplerCreateInfo, nullptr, &mSampler);
    }

    void Sampler::Destroy(Device& device)
    {
        vkDestroySampler(device.GetDevice(), mSampler, nullptr);
    }
}
