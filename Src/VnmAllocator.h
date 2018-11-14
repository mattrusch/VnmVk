// VnmAllocator.h

#pragma once

#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan\vulkan.h"

namespace Vnm
{
    class Allocator
    {
    public:
        Allocator() = default;
        ~Allocator() = default;

        enum MemoryProperties
        {
            DeviceLocal = 1,
            HostVisible = 2,
        };

        void Init(VkPhysicalDevice physicalDevice);
        VkDeviceMemory Allocate(VkDevice device, size_t size, uint32_t memoryRequirementsMask, uint32_t memoryProperties, bool* outHostCoherent = nullptr);
        void Free(VkDevice device, VkDeviceMemory memory);

    private:
        bool mInitialized = false;

        class MemoryDescriptor
        {
        public:
            bool mDeviceLocal = false;
            bool mHostVisible = false;
            bool mHostCoherent = false;
            bool mHostCached = false;
            bool mLazilyAllocated = false;

            class Heap
            {
            public:
                size_t mSize;
                bool   mDeviceLocal = false;
            };

            Heap     mHeap;
            uint32_t mIndex;
        };

        std::vector<MemoryDescriptor> mMemoryDescriptors;
    };
}
