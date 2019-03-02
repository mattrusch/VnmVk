// VnmShader.cpp

#include "VnmShader.h"
#include "VnmFile.h"

namespace Vnm
{
    void Shader::Create(Device& device, const uint32_t* compiledShader, size_t size)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.pCode = compiledShader;
        shaderModuleCreateInfo.codeSize = size;

        vkCreateShaderModule(device.GetDevice(), &shaderModuleCreateInfo, nullptr, &mShaderModule);
    }

    void Shader::CreateFromFile(Device& device, const char* filename)
    {
        FileResource* fileResource = FileResource::LoadFileResource(filename);
        Create(device, reinterpret_cast<const uint32_t*>(fileResource->GetData()), fileResource->GetSize());
        FileResource::DestroyFileResource(fileResource);
    }

    void Shader::Destroy(Device& device)
    {
        vkDestroyShaderModule(device.GetDevice(), mShaderModule, nullptr);
    }
}
