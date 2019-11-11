// VnmImage.h

#pragma once

#include "VnmDevice.h"
#include "VnmAllocator.h"
#include "VnmCommandBuffer.h"
#include "VnmBuffer.h"

namespace Vnm
{
    class Image
    {
    public:
        Image() = default;
        ~Image() = default;

        void CreateDepthStencil(
            Device& device,
            Allocator& allocator,
            int width, 
            int height, 
            VkFormat format);

        void Create2dImage(
            Device& device,
            Allocator& allocator,
            CommandBuffer& uploadCommandBuffer,
            int width,
            int height,
            int numMipLevels,
            VkFormat format,
            uint32_t flags,
            const uint8_t* const* mipImageData,
            const size_t* mipImageDataSizes);

        void Destroy(
            Device& device,
            Allocator& allocator);

        VkImage GetImage() const              { return mImage; }
        VkImageView GetImageView() const      { return mImageView; }
        VkDeviceMemory GetImageMemory() const { return mImageMemory; }
        VkImageLayout GetImageLayout() const  { return mImageLayout; }

    private:
        VkImage        mImage;
        VkImageView    mImageView;
        VkDeviceMemory mImageMemory;
        VkImageLayout  mImageLayout;
        Buffer         mStagingBuffer;
    };
}
