// VnmShader.h

#pragma once

#include "VnmDevice.h"

namespace Vnm
{
    class Shader
    {
    public:
        Shader() = default;
        ~Shader() = default;

        void Create(Device& device, const uint32_t* compiledShader, size_t size);
        void CreateFromFile(Device& device, const char* filename);

        VkShaderModule GetShaderModule() { return mShaderModule; }

    private:
        VkShaderModule mShaderModule;
    };
}
