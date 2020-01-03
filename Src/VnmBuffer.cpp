// VnmBuffer.cpp

#include "VnmBuffer.h"
#include <cassert>

namespace
{
    VkBuffer CreateBuffer(VkDevice device, size_t size, uint32_t usageFlags)
    {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usageFlags;

        VkBuffer result;
        vkCreateBuffer(device, &bufferCreateInfo, nullptr, &result);
        return result;
    }

    void DestroyBuffer(VkDevice device, VkBuffer buffer)
    {
        vkDestroyBuffer(device, buffer, nullptr);
    }

    VkDeviceMemory AllocateDeviceMemory(Vnm::Allocator& allocator, VkDevice device, VkBuffer buffer, uint32_t memoryProperties, bool* isHostCoherent = nullptr)
    {
        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
        VkDeviceSize bufferSize = memoryRequirements.size;
        return allocator.Allocate(device, bufferSize, memoryRequirements.memoryTypeBits, memoryProperties);
    }

    void UpdateDeviceMemory(VkDevice device, VkDeviceMemory deviceMemory, size_t offset, const uint8_t* srcData, size_t srcDataSize, bool isHostCoherent)
    {
        void* dst = nullptr;
        vkMapMemory(device, deviceMemory, offset, VK_WHOLE_SIZE, 0, &dst);
        ::memcpy(dst, srcData, srcDataSize);

        if (!isHostCoherent)
        {
            VkMappedMemoryRange mappedMemoryRange = {};
            mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedMemoryRange.memory = deviceMemory;
            mappedMemoryRange.offset = 0;
            mappedMemoryRange.size = VK_WHOLE_SIZE;

            vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);
        }

        vkUnmapMemory(device, deviceMemory);
    }

    void FreeDeviceMemory(Vnm::Allocator& allocator, VkDevice device, VkDeviceMemory deviceMemory)
    {
        allocator.Free(device, deviceMemory);
    }
}

namespace Vnm
{
    void Buffer::CreateConstantBuffer(Device& device, Allocator& allocator, size_t bufferDataSize)
    {
        // Create uniform buffer
        mBuffer = CreateBuffer(device.GetDevice(), bufferDataSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        bool isHostCoherent = false;
        mBufferMemory = AllocateDeviceMemory(allocator, device.GetDevice(), mBuffer, Allocator::HostVisible, &mIsHostCoherent);
        vkBindBufferMemory(device.GetDevice(), mBuffer, mBufferMemory, 0);
        mBufferType = BufferType::Constant;
    }

    void Buffer::CreateMeshBuffer(Device& device, Allocator& allocator, CommandBuffer& uploadCommandBuffer, const uint8_t* bufferData, size_t bufferDataSize, BufferType bufferType)
    {
        assert(bufferType == BufferType::Index || bufferType == BufferType::Vertex);
        mBufferType = bufferType;

        // Create device buffer
        VkBufferUsageFlags usageFlags = bufferType == BufferType::Index ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        mBuffer = CreateBuffer(device.GetDevice(), bufferDataSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags);
        mBufferMemory = AllocateDeviceMemory(allocator, device.GetDevice(), mBuffer, Allocator::DeviceLocal);
        vkBindBufferMemory(device.GetDevice(), mBuffer, mBufferMemory, 0);

        // Create staging buffer
        CreateStagingBuffer(device, allocator, bufferData, bufferDataSize);

        // Copy data to staging buffer
        UpdateDeviceMemory(device.GetDevice(), mStagingBufferMemory, 0, bufferData, bufferDataSize, mIsHostCoherent);

        // Upload staging buffer to device buffer
        VkBufferCopy bufferCopy = {};
        bufferCopy.size = bufferDataSize;

        vkCmdCopyBuffer(uploadCommandBuffer.GetCommandBuffer(), mStagingBuffer, mBuffer, 1, &bufferCopy);

        VkBufferMemoryBarrier uploadBarrier = {};
        uploadBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        uploadBarrier.buffer = mBuffer;
        uploadBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        uploadBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        uploadBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        uploadBarrier.dstAccessMask = bufferType == BufferType::Index ? VK_ACCESS_INDEX_READ_BIT : VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        uploadBarrier.size = VK_WHOLE_SIZE;

        vkCmdPipelineBarrier(uploadCommandBuffer.GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &uploadBarrier, 0, nullptr);
    }

    void Buffer::Destroy(Device& device, Allocator& allocator)
    {
        if (mStagingBuffer != VK_NULL_HANDLE)
        {
            FreeDeviceMemory(allocator, device.GetDevice(), mStagingBufferMemory);
            DestroyBuffer(device.GetDevice(), mStagingBuffer);
        }

        if (mBuffer != VK_NULL_HANDLE)
        {
            FreeDeviceMemory(allocator, device.GetDevice(), mBufferMemory);
            DestroyBuffer(device.GetDevice(), mStagingBuffer);
        }
    }

    void Buffer::UpdateConstantBuffer(Device& device, const uint8_t* bufferData, size_t bufferDataSize)
    {
        assert(mBufferType == BufferType::Constant);
        UpdateDeviceMemory(device.GetDevice(), mBufferMemory, 0, bufferData, bufferDataSize, mIsHostCoherent);
    }

    void Buffer::CreateStagingBuffer(Device& device, Allocator& allocator, size_t bufferSize)
    {
        // Create staging buffer
        mStagingBuffer = CreateBuffer(device.GetDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        mStagingBufferMemory = AllocateDeviceMemory(allocator, device.GetDevice(), mStagingBuffer, Allocator::HostVisible, &mIsHostCoherent);
        vkBindBufferMemory(device.GetDevice(), mStagingBuffer, mStagingBufferMemory, 0);
    }

    void Buffer::UpdateStagingBuffer(Device& device, size_t offset, const uint8_t* bufferData, size_t bufferDataSize)
    {
        // Copy data to staging buffer
        UpdateDeviceMemory(device.GetDevice(), mStagingBufferMemory, offset, bufferData, bufferDataSize, mIsHostCoherent);
    }

    void Buffer::CreateStagingBuffer(Device& device, Allocator& allocator, const uint8_t* bufferData, size_t bufferDataSize)
    {
        CreateStagingBuffer(device, allocator, bufferDataSize);
        UpdateStagingBuffer(device, 0, bufferData, bufferDataSize);
    }
}
