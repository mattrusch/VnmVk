// VnmSampler

#include "VnmSampler.h"

namespace Vnm
{

    void Sampler::Create(Device& device)
    {
        VkSamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        vkCreateSampler(device.GetDevice(), &samplerCreateInfo, nullptr, &mSampler);
    }
}
