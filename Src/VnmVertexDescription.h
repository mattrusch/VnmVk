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
        void AddInputAttribute(const VkVertexInputAttributeDescription& attribute);

        size_t GetInputBindingDescriptionCount() const                             { return mInputBindingDescriptions.size(); }
        const VkVertexInputBindingDescription* GetInputBindingDescriptions() const { return mInputBindingDescriptions.data(); }

        size_t GetInputAttributeDescriptionCount() const                               { return mInputAttributeDescriptions.size(); }
        const VkVertexInputAttributeDescription* GetInputAttributeDescriptions() const { return mInputAttributeDescriptions.data(); }

    private:
        std::vector<VkVertexInputBindingDescription>  mInputBindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> mInputAttributeDescriptions;
    };
}
