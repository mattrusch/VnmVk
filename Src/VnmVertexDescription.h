// VnmVertexDescription.h

#pragma once

#include "vulkan/vulkan.h"
#include <vector>

namespace Vnm
{
    class VertexDescription
    {
    public:
        VertexDescription() = default;
        ~VertexDescription() = default;

        void AddInputBinding(const VkVertexInputBindingDescription& binding);
        void AddInputBinding(
            uint32_t binding, 
            uint32_t stride,
            VkVertexInputRate inputRate);

        void AddInputAttribute(const VkVertexInputAttributeDescription& attribute);
        void AddInputAttribute(
            uint32_t location,
            uint32_t binding,
            VkFormat format,
            uint32_t offset);

        size_t GetInputBindingDescriptionCount() const                             { return mInputBindingDescriptions.size(); }
        const VkVertexInputBindingDescription* GetInputBindingDescriptions() const { return mInputBindingDescriptions.data(); }

        size_t GetInputAttributeDescriptionCount() const                               { return mInputAttributeDescriptions.size(); }
        const VkVertexInputAttributeDescription* GetInputAttributeDescriptions() const { return mInputAttributeDescriptions.data(); }

    private:
        std::vector<VkVertexInputBindingDescription>  mInputBindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> mInputAttributeDescriptions;
    };
}
