// VnmSampler

#include "VnmSampler.h"

namespace Vnm
{

    void Sampler::Create(Device& device)
    {
        VkSamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
        samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        vkCreateSampler(device.GetDevice(), &samplerCreateInfo, nullptr, &mSampler);
    }

    void Sampler::Destroy(Device& device)
    {
        vkDestroySampler(device.GetDevice(), mSampler, nullptr);
    }
}
