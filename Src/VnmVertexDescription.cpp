#include "VnmVertexDescription.h"

namespace Vnm
{
    void VertexDescription::AddInputBinding(const VkVertexInputBindingDescription& binding)
    {
        mInputBindingDescriptions.emplace_back(binding);
    }

    void VertexDescription::AddInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
    {
        mInputBindingDescriptions.emplace_back();
        mInputBindingDescriptions.back().binding = binding;
        mInputBindingDescriptions.back().inputRate = inputRate;
        mInputBindingDescriptions.back().stride = stride;
    }

    void VertexDescription::AddInputAttribute(const VkVertexInputAttributeDescription& attribute)
    {
        mInputAttributeDescriptions.emplace_back(attribute);
    }

    void VertexDescription::AddInputAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset)
    {
        mInputAttributeDescriptions.emplace_back();
        mInputAttributeDescriptions.back().location = location;
        mInputAttributeDescriptions.back().binding = binding;
        mInputAttributeDescriptions.back().format = format;
        mInputAttributeDescriptions.back().offset = offset;
    }

}
