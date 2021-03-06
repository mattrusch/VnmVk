// VnmImage.cpp

#include "VnmImage.h"
#include <cassert>
#include <vector>

namespace
{
    void InitVkImageCreateInfo(
        VkImageCreateInfo& dstVkImageCreateInfo, 
        VkFormat format, 
        uint32_t width, 
        uint32_t height,
        uint32_t numMipLevels,
        VkImageUsageFlags usage, 
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL)
    {
        dstVkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        dstVkImageCreateInfo.pNext = nullptr;
        dstVkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        dstVkImageCreateInfo.format = format;
        dstVkImageCreateInfo.extent.width = width;
        dstVkImageCreateInfo.extent.height = height;
        dstVkImageCreateInfo.extent.depth = 1;
        dstVkImageCreateInfo.mipLevels = numMipLevels;
        dstVkImageCreateInfo.arrayLayers = 1;
        dstVkImageCreateInfo.samples = samples;
        dstVkImageCreateInfo.tiling = tiling;
        dstVkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        dstVkImageCreateInfo.usage = usage;
        dstVkImageCreateInfo.queueFamilyIndexCount = 0;
        dstVkImageCreateInfo.pQueueFamilyIndices = nullptr;
        dstVkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        dstVkImageCreateInfo.flags = 0;
    }

    void SetImageMemoryBarrier(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageAspectFlags aspectMask,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkPipelineStageFlags srcStages,
        VkPipelineStageFlags dstStages,
        uint32_t baseMipLevel = 0,
        uint32_t mipLevelCount = 1)
    {
        VkImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext = nullptr;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = 0;
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
        imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
        imageMemoryBarrier.subresourceRange.levelCount = mipLevelCount;
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        switch (oldImageLayout)
        {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default: break;
        }

        switch (newImageLayout)
        {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default: break;
        }

        vkCmdPipelineBarrier(commandBuffer, srcStages, dstStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    }
}

namespace Vnm
{
    void Image::CreateDepthStencil(Device& device, Allocator& allocator, int width, int height, VkFormat format)
    {
        assert(format == VK_FORMAT_D16_UNORM);

        VkImageCreateInfo imageCreateInfo = {};
        InitVkImageCreateInfo(imageCreateInfo, format, width, height, 1, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

        vkCreateImage(device.GetDevice(), &imageCreateInfo, nullptr, &mImage);

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device.GetDevice(), mImage, &memoryRequirements);
        mImageMemory = allocator.Allocate(device.GetDevice(), memoryRequirements.size, memoryRequirements.memoryTypeBits, Allocator::DeviceLocal);

        vkBindImageMemory(device.GetDevice(), mImage, mImageMemory, 0);
        
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.image = mImage;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.flags = 0;

        vkCreateImageView(device.GetDevice(), &imageViewCreateInfo, nullptr, &mImageView);
    }

    void Image::Create2dImage(
        Device& device, 
        Allocator& allocator, 
        CommandBuffer& uploadCommandBuffer, 
        int width, 
        int height, 
        int numMipLevels, 
        VkFormat format, 
        uint32_t flags, 
        const uint8_t* const* mipImageData, 
        const size_t* mipImageDataSizes)
    {
        // Calculate staging buffer size
        size_t totalMipDataSize = 0;
        for (int i = 0; i < numMipLevels; ++i)
        {
            totalMipDataSize += mipImageDataSizes[i];
        }

        // Create staging buffer
        mStagingBuffer.CreateStagingBuffer(device, allocator, totalMipDataSize);

        // Copy mips into staging buffer and set up buffer image copy descriptors
        size_t offset = 0;
        std::vector<VkBufferImageCopy> bufferImageCopies(numMipLevels);
        for (int i = 0; i < numMipLevels; ++i)
        {
            mStagingBuffer.UpdateStagingBuffer(device, offset, mipImageData[i], mipImageDataSizes[i]);

            bufferImageCopies[i].bufferOffset = offset;
            bufferImageCopies[i].imageExtent.width = width >> i;
            bufferImageCopies[i].imageExtent.height = height >> i;
            bufferImageCopies[i].imageExtent.depth = 1;
            bufferImageCopies[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            bufferImageCopies[i].imageSubresource.layerCount = 1;
            bufferImageCopies[i].imageSubresource.mipLevel = i;
            bufferImageCopies[i].imageSubresource.baseArrayLayer = 0;
            bufferImageCopies[i].imageOffset.x = 0;
            bufferImageCopies[i].imageOffset.y = 0;
            bufferImageCopies[i].imageOffset.z = 0;
            bufferImageCopies[i].bufferRowLength = width >> i;
            bufferImageCopies[i].bufferImageHeight = height >> i;

            offset += mipImageDataSizes[i];
        }

        // Create device image
        VkImageCreateInfo imageCreateInfo;
        InitVkImageCreateInfo(imageCreateInfo, format, width, height, numMipLevels, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        vkCreateImage(device.GetDevice(), &imageCreateInfo, nullptr, &mImage);

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device.GetDevice(), mImage, &memoryRequirements);
        mImageMemory = allocator.Allocate(device.GetDevice(), memoryRequirements.size, memoryRequirements.memoryTypeBits, Allocator::DeviceLocal);

        vkBindImageMemory(device.GetDevice(), mImage, mImageMemory, 0);

        // Upload staging image to device image
        SetImageMemoryBarrier(
            uploadCommandBuffer.GetCommandBuffer(),
            mImage,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            numMipLevels);

        vkCmdCopyBufferToImage(uploadCommandBuffer.GetCommandBuffer(), mStagingBuffer.GetStagingBuffer(), mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferImageCopies.size()), bufferImageCopies.data());

        mImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        SetImageMemoryBarrier(
            uploadCommandBuffer.GetCommandBuffer(),
            mImage,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            mImageLayout,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            numMipLevels);

        // Create image view
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.image = mImage;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.levelCount = static_cast<uint32_t>(bufferImageCopies.size());
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        vkCreateImageView(device.GetDevice(), &imageViewCreateInfo, nullptr, &mImageView);
    }

    void Image::Destroy(Device& device, Allocator& allocator)
    {
        mStagingBuffer.Destroy(device, allocator);
        allocator.Free(device.GetDevice(), mImageMemory);
        vkDestroyImageView(device.GetDevice(), mImageView, nullptr);
        vkDestroyImage(device.GetDevice(), mImage, nullptr);
    }
}
