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
        void Destroy(Device& device);

        VkCommandBuffer& GetCommandBuffer() { return mCommandBuffer; }
        VkCommandBuffer* GetCommandBufferPtr() { return &mCommandBuffer; }

    private:
        VkCommandBuffer mCommandBuffer;
    };
}
