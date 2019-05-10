#include "VnmVertexDescription.h"

namespace Vnm
{
    void VertexDescription::AddInputBinding(const VkVertexInputBindingDescription& binding)
    {
        mInputBindingDescriptions.emplace_back(binding);
    }

    void VertexDescription::AddInputAttribute(const VkVertexInputAttributeDescription& attribute)
    {
        mInputAttributeDescriptions.emplace_back(attribute);
    }
}
