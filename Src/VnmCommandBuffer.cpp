// VnmCommandBuffer.cpp

#include "VnmCommandBuffer.h"

namespace Vnm
{
    void CommandBuffer::Create(VkCommandBuffer commandBuffer)
    {
        mCommandBuffer = commandBuffer;
    }

    void CommandBuffer::Destroy(Device& device)
    {
    }
}
