// VnmDevice.cpp

#include "VnmDevice.h"
#include <vector>
#include <cassert>

namespace
{
    // Error reporting stolen, with slight modification, from AMD sample
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
        VkDebugReportFlagsEXT       /*flags*/,
        VkDebugReportObjectTypeEXT  /*objectType*/,
        uint64_t                    /*object*/,
        size_t                      /*location*/,
        int32_t                     /*messageCode*/,
        const char*                 /*pLayerPrefix*/,
        const char*                 pMessage,
        void*                       /*pUserData*/)
    {
        OutputDebugStringA(pMessage);
        OutputDebugStringA("\n");
        return VK_FALSE;
    }

    class ImportTable
    {
    public:
        #define GET_INSTANCE_ENTRYPOINT(i, w) w = reinterpret_cast<PFN_##w>(vkGetInstanceProcAddr(i, #w))
        #define GET_DEVICE_ENTRYPOINT(i, w) w = reinterpret_cast<PFN_##w>(vkGetDeviceProcAddr(i, #w))

        ImportTable() = default;
        void Reset(VkInstance instance, VkDevice device)
        {
            #ifdef _DEBUG
            GET_INSTANCE_ENTRYPOINT(instance, vkCreateDebugReportCallbackEXT);
            GET_INSTANCE_ENTRYPOINT(instance, vkDebugReportMessageEXT);
            GET_INSTANCE_ENTRYPOINT(instance, vkDestroyDebugReportCallbackEXT);
            #endif //_DEBUG
        }

        #ifdef _DEBUG
        PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = nullptr;
        PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT = nullptr;
        PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
        #endif
    };

    #ifdef _DEBUG
    VkDebugReportCallbackEXT SetupDebugCallback(VkInstance instance, ImportTable* importTable)
    {
        if (importTable->vkCreateDebugReportCallbackEXT)
        {
            VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
            callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
            callbackCreateInfo.flags =
                VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                VK_DEBUG_REPORT_DEBUG_BIT_EXT |
                VK_DEBUG_REPORT_ERROR_BIT_EXT |
                VK_DEBUG_REPORT_WARNING_BIT_EXT |
                VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
            callbackCreateInfo.pfnCallback = &DebugReportCallback;

            VkDebugReportCallbackEXT callback;
            importTable->vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);
            return callback;
        }
        else
        {
            return VK_NULL_HANDLE;
        }
    }

    void CleanupDebugCallback(VkInstance instance, VkDebugReportCallbackEXT callback, ImportTable* importTable)
    {
        if (importTable->vkDestroyDebugReportCallbackEXT)
        {
            importTable->vkDestroyDebugReportCallbackEXT(instance, callback, nullptr);
        }
    }
    #endif //_DEBUG

    VkInstance CreateInstance()
    {
        std::vector<const char*> extensionNames = { "VK_KHR_surface", "VK_KHR_win32_surface" };

        #ifdef _DEBUG
        extensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        #endif //_DEBUG

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());

        std::vector<const char*> layerNames;

        #ifdef _DEBUG
        layerNames.push_back("VK_LAYER_LUNARG_standard_validation");
        #endif //_DEBUG

        instanceCreateInfo.ppEnabledLayerNames = layerNames.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layerNames.size());

        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pApplicationName = "VnmVk";
        applicationInfo.pEngineName = "Vnm";

        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        VkInstance instance = VK_NULL_HANDLE;
        VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        assert(res == VK_SUCCESS);

        return instance;
    }

    void FindPhysicalDeviceWithGraphicsQueue(
        const std::vector<VkPhysicalDevice>& physicalDevices,
        VkPhysicalDevice* outDevice,
        int* outGraphicsQueueIndex)
    {
        for (auto physicalDevice : physicalDevices)
        {
            uint32_t queueFamilyPropertyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

            int i = 0;
            for (const auto& queueFamilyProperty : queueFamilyProperties)
            {
                if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    if (outDevice != nullptr)
                    {
                        *outDevice = physicalDevice;
                    }

                    if (outGraphicsQueueIndex != nullptr)
                    {
                        *outGraphicsQueueIndex = i;
                    }

                    return;
                }

                ++i;
            }
        }

        assert(!"Failed to find physcial device");
    }

    void CreateDeviceAndQueue(
        VkInstance instance,
        VkDevice* outDevice,
        VkQueue* outQueue,
        int* outQueueIndex,
        VkPhysicalDevice* outPhysicalDevice)
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

        std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, devices.data());

        VkPhysicalDevice physicalDevice = nullptr;
        int graphicsQueueIndex = -1;
        FindPhysicalDeviceWithGraphicsQueue(devices, &physicalDevice, &graphicsQueueIndex);
        assert(physicalDevice != nullptr);

        VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueIndex;

        static const float queuePriorities[] = { 1.0f };
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

        // TODO: Debug device layer names
        std::vector<const char*> deviceLayerNames;

        #ifdef _DEBUG
        deviceLayerNames.push_back("VK_LAYER_LUNARG_standard_validation");
        #endif //_DEBUG

        deviceCreateInfo.ppEnabledLayerNames = deviceLayerNames.data();
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(deviceLayerNames.size());

        std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" };
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

        VkDevice device = nullptr;
        vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
        assert(device != nullptr);

        VkQueue queue = nullptr;
        vkGetDeviceQueue(device, graphicsQueueIndex, 0, &queue);
        assert(queue);

        if (outQueue != nullptr)
        {
            *outQueue = queue;
        }

        if (outDevice != nullptr)
        {
            *outDevice = device;
        }

        if (outQueueIndex != nullptr)
        {
            *outQueueIndex = graphicsQueueIndex;
        }

        if (outPhysicalDevice != nullptr)
        {
            *outPhysicalDevice = physicalDevice;
        }
    }

}

namespace Vnm
{
    void Device::Create()
    {
        mInstance = CreateInstance();
        if (mInstance == VK_NULL_HANDLE)
        {
            assert(!"Unable to create Vulkan instance");
            return;
        }

        CreateDeviceAndQueue(mInstance, &mDevice, &mQueue, &mQueueFamilyIndex, &mPhysicalDevice);

        #ifdef _DEBUG
        ImportTable importTable;
        importTable.Reset(mInstance, mDevice);
        SetupDebugCallback(mInstance, &importTable);
        #endif //_DEBUG
    }
}
