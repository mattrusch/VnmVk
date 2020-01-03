// VnmRenderContext.cpp

#include "VnmRenderContext.h"
#include <cassert>

namespace
{
    VkSurfaceKHR CreateSurface(
        VkInstance instance,
        HWND hwnd)
    {
        VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
        win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        win32SurfaceCreateInfo.hwnd = hwnd;
        win32SurfaceCreateInfo.hinstance = ::GetModuleHandle(nullptr);

        VkSurfaceKHR surface;
        vkCreateWin32SurfaceKHR(instance, &win32SurfaceCreateInfo, nullptr, &surface);

        return surface;
    }

    VkRenderPass CreateRenderPass(
        VkDevice device,
        VkFormat swapChainFormat,
        VkFormat depthFormat)
    {
        VkAttachmentDescription attachmentDescriptions[2] = {};
        attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescriptions[0].format = swapChainFormat;
        attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescriptions[1].format = depthFormat;
        attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentReference = {};
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentReference;
        subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 2;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.pAttachments = &attachmentDescriptions[0];

        VkRenderPass result;
        vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &result);
        return result;
    }

    void CreateFramebuffers(
        VkDevice device,
        VkRenderPass renderPass,
        int width,
        int height,
        int count,
        const VkImageView* imageViews,
        VkImageView depthView,
        VkFramebuffer* outFramebuffers)
    {
        VkImageView attachments[2];
        attachments[1] = depthView;

        for (int i = 0; i < count; ++i)
        {
            attachments[0] = imageViews[i];

            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.attachmentCount = 2;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.height = height;
            framebufferCreateInfo.width = width;
            framebufferCreateInfo.layers = 1;
            framebufferCreateInfo.renderPass = renderPass;

            vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &outFramebuffers[i]);
        }
    }

    void DestroyFramebuffers(
        VkDevice device, 
        VkFramebuffer* framebuffers, 
        int count)
    {
        for (int i = 0; i < count; ++i)
        {
            vkDestroyFramebuffer(device, framebuffers[i], nullptr);
        }
    }
}

namespace Vnm
{
    const VkFormat depthFormat = VK_FORMAT_D16_UNORM;

    void RenderContext::Init(HWND hwnd, int width, int height)
    {
        mDevice.Create();

        mAllocator.Init(mDevice.GetPhysicalDevice());

        mSurface = CreateSurface(mDevice.GetInstance(), hwnd);
        VkBool32 presentSupported;
        vkGetPhysicalDeviceSurfaceSupportKHR(mDevice.GetPhysicalDevice(), 0, mSurface, &presentSupported);
        assert(presentSupported);

        mSwapChain.Create(mDevice, mSurface, width, height, mBackbufferCount);
        mDepthBuffer.CreateDepthStencil(mDevice, mAllocator, width, height, depthFormat);
        mRenderPass = CreateRenderPass(mDevice.GetDevice(), mSwapChain.GetFormat(), depthFormat);
        CreateFramebuffers(mDevice.GetDevice(), mRenderPass, width, height, mBackbufferCount, mSwapChain.GetImageViews(), mDepthBuffer.GetImageView(), mFramebuffers);

        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = mDevice.GetQueueFamilyIndex();
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        vkCreateCommandPool(mDevice.GetDevice(), &commandPoolCreateInfo, nullptr, &mCommandPool);

        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandBufferCount = mBackbufferCount + 1;
        commandBufferAllocateInfo.commandPool = mCommandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkCommandBuffer commandBuffers[mBackbufferCount + 1];
        vkAllocateCommandBuffers(mDevice.GetDevice(), &commandBufferAllocateInfo, commandBuffers);

        for (int i = 0; i < mBackbufferCount; ++i)
        {
            mCommandBuffers[i].Create(commandBuffers[i]);
        }

        mUploadCommandBuffer.Create(commandBuffers[mBackbufferCount]);

        for (int i = 0; i < mBackbufferCount; ++i)
        {
            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            //fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            vkCreateFence(mDevice.GetDevice(), &fenceCreateInfo, nullptr, &mFences[i]);
        }

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(mDevice.GetDevice(), &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore);
        vkCreateSemaphore(mDevice.GetDevice(), &semaphoreCreateInfo, nullptr, &mRenderingCompleteSemaphore);
    }

    void RenderContext::Run(int width, int height)
    {
        BeginPass(width, height);
        EndPass();
    }

    void RenderContext::BeginPass(int width, int height)
    {
        assert(mDevice.GetInstance() != VK_NULL_HANDLE);

        vkAcquireNextImageKHR(mDevice.GetDevice(), mSwapChain.GetSwapchain(), UINT64_MAX, mImageAcquiredSemaphore, VK_NULL_HANDLE, &mCurrentBackbufferIndex);

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(mCommandBuffers[mCurrentBackbufferIndex].GetCommandBuffer(), &commandBufferBeginInfo);

        VkClearValue clearValues[2];
        clearValues[0].color.float32[0] = 0.65f;
        clearValues[0].color.float32[1] = 0.75f;
        clearValues[0].color.float32[2] = 1.0f;
        clearValues[0].color.float32[3] = 1.0f;
        clearValues[1].depthStencil.depth = 1.0f;
        clearValues[1].depthStencil.stencil= 0;

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.framebuffer = mFramebuffers[mCurrentBackbufferIndex];
        renderPassBeginInfo.renderArea.extent.width = width;
        renderPassBeginInfo.renderArea.extent.height = height;
        renderPassBeginInfo.renderPass = mRenderPass;
        renderPassBeginInfo.pClearValues = clearValues;
        renderPassBeginInfo.clearValueCount = 2;

        vkCmdBeginRenderPass(mCommandBuffers[mCurrentBackbufferIndex].GetCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderContext::EndPass()
    {
        vkCmdEndRenderPass(mCommandBuffers[mCurrentBackbufferIndex].GetCommandBuffer());
        vkEndCommandBuffer(mCommandBuffers[mCurrentBackbufferIndex].GetCommandBuffer());

        // Submit to grahics queue
        const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &mImageAcquiredSemaphore;
        submitInfo.pWaitDstStageMask = &waitDstStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = mCommandBuffers[mCurrentBackbufferIndex].GetCommandBufferPtr();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &mRenderingCompleteSemaphore;
        vkQueueSubmit(mDevice.GetQueue(), 1, &submitInfo, VK_NULL_HANDLE);

        // Submit to present queue
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &mRenderingCompleteSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = mSwapChain.GetSwapchainPtr();
        presentInfo.pImageIndices = &mCurrentBackbufferIndex;
        vkQueuePresentKHR(mDevice.GetQueue(), &presentInfo);

        vkQueueSubmit(mDevice.GetQueue(), 0, nullptr, mFences[mCurrentBackbufferIndex]);
        vkWaitForFences(mDevice.GetDevice(), 1, &mFences[mCurrentBackbufferIndex], VK_TRUE, UINT64_MAX);
        vkResetFences(mDevice.GetDevice(), 1, &mFences[mCurrentBackbufferIndex]);
    }

    void RenderContext::Shutdown()
    {
        assert(!"Stubbed - complete this function");
    }
}
