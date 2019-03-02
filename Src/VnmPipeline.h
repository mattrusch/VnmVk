// VnmPipeline.h

#pragma once

#include "VnmDevice.h"

namespace Vnm
{
    class Pipeline
    {
    public:
        Pipeline() = default;
        ~Pipeline() = default;

        void Create(
            Device& device,
            VkRenderPass renderPass,
            VkPipelineLayout layout,
            VkShaderModule vertexShader,
            VkShaderModule fragmentShader);
        
        void Destroy(Device& device);

        VkPipeline& GetPipeline() { return mPipeline; }

    private:
        VkPipeline mPipeline;
    };
}
