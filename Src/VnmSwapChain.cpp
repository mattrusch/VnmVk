// VnmSwapChain.cpp

#include "VnmSwapChain.h"
#include <cassert>
#include <vector>

namespace
{
    void GetSwapChainFormatAndColorSpace(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        VkFormat* outFormat,
        VkColorSpaceKHR* outColorSpace)
    {
        uint32_t surfaceFormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());

        if (surfaceFormatCount == 1 && surfaceFormats.front().format == VK_FORMAT_UNDEFINED)
        {
            *outFormat = VK_FORMAT_R8G8B8A8_UNORM;
        }
        else
        {
            *outFormat = surfaceFormats.front().format;
        }

        *outColorSpace = surfaceFormats.front().colorSpace;
    }

    void CreateFramebuffers(
        VkDevice device,
        VkRenderPass renderPass,
        int width,
        int height,
        int count,
        const VkImageView* imageViews,
        VkFramebuffer* outFramebuffers)
    {
        for (int i = 0; i < count; ++i)
        {
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = &imageViews[i];
            framebufferCreateInfo.height = height;
            framebufferCreateInfo.width = width;
            framebufferCreateInfo.layers = 1;
            framebufferCreateInfo.renderPass = renderPass;

            vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &outFramebuffers[i]);
        }
    }

    void CreateSwapchainImageViews(
        VkDevice device,
        VkFormat format,
        int count,
        const VkImage* images,
        VkImageView* outImageViews)
    {
        for (int i = 0; i < count; ++i)
        {
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = images[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = format;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.layerCount = 1;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

            vkCreateImageView(device, &imageViewCreateInfo, nullptr, &outImageViews[i]);
        }
    }

    VkSwapchainKHR CreateSwapchain(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkSurfaceKHR surface,
        int surfaceWidth,
        int surfaceHeight,
        int backbufferCount,
        VkFormat* outSwapchainFormat)
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

        VkExtent2D swapchainSize = {};
        swapchainSize = surfaceCapabilities.currentExtent;
        assert(swapchainSize.width == surfaceWidth);
        assert(swapchainSize.height == surfaceHeight);

        uint32_t swapchainImageCount = backbufferCount;
        assert(swapchainImageCount >= surfaceCapabilities.minImageCount);

        // maxImageCount of 0 means unlimited number of images
        assert((surfaceCapabilities.maxImageCount != 0) && (swapchainImageCount < surfaceCapabilities.maxImageCount));

        VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits =
            surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform;

        VkFormat swapchainFormat;
        VkColorSpaceKHR swapchainColorSpace;
        GetSwapChainFormatAndColorSpace(physicalDevice, surface, &swapchainFormat, &swapchainColorSpace);

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.minImageCount = swapchainImageCount;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = surfaceTransformFlagBits;
        swapchainCreateInfo.imageColorSpace = swapchainColorSpace;
        swapchainCreateInfo.imageFormat = swapchainFormat;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.imageExtent = swapchainSize;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

        VkSwapchainKHR swapchain;
        vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);

        if (outSwapchainFormat != nullptr)
        {
            *outSwapchainFormat = swapchainFormat;
        }

        return swapchain;
    }
}

namespace Vnm
{
    void SwapChain::Create(Device& device, VkSurfaceKHR surface, int width, int height, int backbufferCount)
    {
        mSwapchain = CreateSwapchain(device.GetPhysicalDevice(), device.GetDevice(), surface, width, height, backbufferCount, &mSwapchainFormat);
        assert(mSwapchain);

        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(device.GetDevice(), mSwapchain, &swapchainImageCount, nullptr);
        assert(swapchainImageCount == backbufferCount);
        mSwapchainImages.reserve(backbufferCount);
        vkGetSwapchainImagesKHR(device.GetDevice(), mSwapchain, &swapchainImageCount, mSwapchainImages.data());

        mSwapchainImageViews.reserve(backbufferCount);
        CreateSwapchainImageViews(device.GetDevice(), mSwapchainFormat, backbufferCount, mSwapchainImages.data(), mSwapchainImageViews.data());
    }
}
