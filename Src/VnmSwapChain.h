// VnmSwapChain.h

#pragma once

#include "VnmDevice.h"
#include <vector>

namespace Vnm
{
    class SwapChain
    {
    public:
        SwapChain() = default;
        ~SwapChain() = default;

        void Create(
            Device& device, 
            VkSurfaceKHR surface, 
            int width, 
            int height, 
            int backbufferCount);

        void Destroy(Device& device);

        VkSwapchainKHR GetSwapchain() const           { return mSwapchain; }
        const VkSwapchainKHR* GetSwapchainPtr() const { return &mSwapchain; }
        VkFormat GetFormat() const                    { return mSwapchainFormat; }
        const VkImageView* GetImageViews() const      { return mSwapchainImageViews.data(); }

    private:
        VkSwapchainKHR           mSwapchain           = VK_NULL_HANDLE;
        VkFormat                 mSwapchainFormat     = VK_FORMAT_UNDEFINED;
        std::vector<VkImage>     mSwapchainImages;
        std::vector<VkImageView> mSwapchainImageViews;
    };
}
