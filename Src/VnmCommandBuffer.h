// VnmCommandBuffer.h

#pragma once

#include "VnmDevice.h"

namespace Vnm
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        ~CommandBuffer() = default;

        void Create(VkCommandBuffer commandBuffer);

        VkCommandBuffer& GetCommandBuffer() { return mCommandBuffer; }
        VkCommandBuffer* GetCommandBufferPtr() { return &mCommandBuffer; }

    private:
        VkCommandBuffer mCommandBuffer;
    };
}
