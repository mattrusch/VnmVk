// VnmRenderContext.h

#pragma once

#include "VnmDevice.h"
#include "VnmSwapChain.h"
#include "VnmCommandBuffer.h"
#include "VnmAllocator.h"
#include "VnmImage.h"

namespace Vnm
{
    class RenderContext
    {
    public:
        void Init(HWND hwnd, int width, int height);
        void Shutdown();
        void Run(int width, int height);
        void BeginPass(int width, int height);
        void EndPass();

        Device& GetDevice()                      { return mDevice; }
        Allocator& GetAllocator()                { return mAllocator; }
        VkRenderPass& GetRenderPass()            { return mRenderPass; }
        CommandBuffer& GetUploadCommandBuffer()  { return mUploadCommandBuffer;  }
        CommandBuffer& GetCurrentCommandBuffer() { return mCommandBuffers[mCurrentBackbufferIndex]; }
        VkFence& GetFence(int index)             { return mFences[index];  }

    private:
        static const int mBackbufferCount = 3;

        Device    mDevice;
        SwapChain mSwapChain;
        Image     mDepthBuffer;
        Allocator mAllocator;

        VkSurfaceKHR  mSurface;
        VkRenderPass  mRenderPass;
        VkFramebuffer mFramebuffers[mBackbufferCount];

        VkCommandPool mCommandPool;
        CommandBuffer mCommandBuffers[mBackbufferCount];
        CommandBuffer mUploadCommandBuffer;
        uint32_t      mCurrentBackbufferIndex = 0;

        VkFence     mFences[mBackbufferCount];
        VkSemaphore mImageAcquiredSemaphore;
        VkSemaphore mRenderingCompleteSemaphore;
    };
}
