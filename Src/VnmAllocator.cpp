// VnmAllocator.cpp

#include "VnmAllocator.h"
#include <vector>
#include <cassert>

namespace Vnm
{
    void Allocator::Init(VkPhysicalDevice physicalDevice)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties = {};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        std::vector<MemoryDescriptor::Heap> heaps;
        for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; ++i)
        {
            const VkMemoryHeap& memoryHeap = memoryProperties.memoryHeaps[i];
            heaps.emplace_back(MemoryDescriptor::Heap{ memoryHeap.size, (memoryHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0 });
        }

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
        {
            const VkMemoryType& memoryType = memoryProperties.memoryTypes[i];
            mMemoryDescriptors.emplace_back(
                MemoryDescriptor
                {
                    (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0,
                    (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0,
                    (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0,
                    (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0,
                    (memoryType.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0,
                    heaps[memoryType.heapIndex],
                    i
                });
        }

        mInitialized = true;
    }

    VkDeviceMemory Allocator::Allocate(VkDevice device, size_t size, uint32_t memoryRequirementsMask, uint32_t memoryProperties, bool* outHostCoherent /*= nullptr*/)
    {
        assert(mInitialized);

        for (const auto& descriptor : mMemoryDescriptors)
        {
            if (((1 << descriptor.mIndex) & memoryRequirementsMask) == 0)
            {
                continue;
            }

            if ((memoryProperties & DeviceLocal) && !descriptor.mDeviceLocal)
            {
                continue;
            }

            if ((memoryProperties & HostVisible) && !descriptor.mHostVisible)
            {
                continue;
            }

            if (outHostCoherent != nullptr)
            {
                *outHostCoherent = descriptor.mHostCoherent;
            }

            VkMemoryAllocateInfo memoryAllocateInfo = {};
            memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memoryAllocateInfo.memoryTypeIndex = descriptor.mIndex;
            memoryAllocateInfo.allocationSize = size;

            VkDeviceMemory deviceMemory;
            vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &deviceMemory);
            return deviceMemory;
        }

        return VK_NULL_HANDLE;
    }

    void Allocator::Free(VkDevice device, VkDeviceMemory memory)
    {
        vkFreeMemory(device, memory, nullptr);
    }
}
