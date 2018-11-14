// VnmBuffer.h

#pragma once

#include "VnmDevice.h"
#include "VnmAllocator.h"
#include "VnmCommandBuffer.h"

namespace Vnm
{
    class Buffer
    {
    public:
        Buffer() = default;
        ~Buffer() = default;

        enum class BufferType
        {
            Index,
            Vertex,
            Constant,
            Staging,
            NumBufferTypes
        };

        void CreateConstantBuffer(
            Device& device,
            Allocator& allocator,
            size_t bufferDataSize);

        void CreateMeshBuffer(
            Device& device, 
            Allocator& allocator, 
            CommandBuffer& uploadCommandBuffer,
            const uint8_t* bufferData, 
            size_t bufferDataSize,
            BufferType bufferType);

        void CreateStagingBuffer(
            Device& device,
            Allocator& allocator,
            const uint8_t* bufferData,
            size_t bufferDataSize);

        void Destroy();

        void UpdateConstantBuffer(
            Device& device,
            const uint8_t* bufferData,
            size_t bufferDataSize);

        VkBuffer GetBuffer()        { return mBuffer; }
        VkBuffer* GetBufferPtr()    { return &mBuffer; }
        VkBuffer GetStagingBuffer() { return mStagingBuffer;  }

    private:
        VkBuffer       mBuffer              = VK_NULL_HANDLE;
        VkDeviceMemory mBufferMemory        = VK_NULL_HANDLE;
        VkBuffer       mStagingBuffer       = VK_NULL_HANDLE;
        VkDeviceMemory mStagingBufferMemory = VK_NULL_HANDLE;
        size_t         mBufferSize          = 0;
        size_t         mStagingBufferSize   = 0;
        BufferType     mBufferType;
        bool           mIsHostCoherent;
    };
}
