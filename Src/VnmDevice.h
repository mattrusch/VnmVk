// VnmDevice.h

#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

namespace Vnm
{
    class Device
    {
    public:
        Device() = default;
        ~Device() = default;

        void Create();
        void Destroy(); // TODO

        VkInstance GetInstance() const             { return mInstance; }
        VkPhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; }
        VkDevice GetDevice() const                 { return mDevice; }
        VkQueue GetQueue() const                   { return mQueue; }
        int GetQueueFamilyIndex() const            { return mQueueFamilyIndex; }

    private:
        VkInstance       mInstance         = VK_NULL_HANDLE;
        VkPhysicalDevice mPhysicalDevice   = VK_NULL_HANDLE;
        VkDevice         mDevice           = VK_NULL_HANDLE;
        VkQueue          mQueue            = VK_NULL_HANDLE;
        int              mQueueFamilyIndex = -1;
    };
}
