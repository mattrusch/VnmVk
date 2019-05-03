// VnmVk.cpp

#include "Window.h"
#include "crtdbg.h"
#include "VnmRenderContext.h"
#include "VnmShader.h"
#include "VnmPipeline.h"
#include "VnmPipelineLayout.h"
#include "VnmBuffer.h"
#include "VnmDescriptorSet.h"
#include "VnmDescriptorSetLayout.h"
#include "VnmDescriptorPool.h"
#include "VnmSampler.h"
#include "VnmObjMesh.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace Vnm
{
    class Vertex
    {
    public:
        float position[3];
        float uv[2];
    };

    const Vertex quadVertices[4] =
    {
        {-0.5f, 0.0f,  0.5f, 0.0f, 1.0f},
        { 0.5f, 0.0f,  0.5,  1.0f, 1.0f},
        { 0.5f, 0.0f, -0.5f, 1.0f, 0.0f},
        {-0.5f, 0.0f, -0.5f, 0.0f, 0.0f}
    };

    const Vertex cubeVertices[24] =
    {
        { -0.5f,  0.5f, 0.5f, 0.0f, 1.0f },
        {  0.5f,  0.5,  0.5f, 1.0f, 1.0f },
        {  0.5f, -0.5f, 0.5f, 1.0f, 0.0f },
        { -0.5f, -0.5f, 0.5f, 0.0f, 0.0f },

        { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
        {  0.5f,  0.5,  -0.5f, 1.0f, 1.0f },
        {  0.5f, -0.5f, -0.5f, 1.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f },

        { -0.5f, 0.5f,  0.5f, 0.0f, 1.0f },
        {  0.5f, 0.5f,  0.5,  1.0f, 1.0f },
        {  0.5f, 0.5f, -0.5f, 1.0f, 0.0f },
        { -0.5f, 0.5f, -0.5f, 0.0f, 0.0f },

        { -0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
        {  0.5f, -0.5f,  0.5,  1.0f, 1.0f },
        {  0.5f, -0.5f, -0.5f, 1.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f },

        { 0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
        { 0.5f,  0.5f,  0.5,  1.0f, 1.0f },
        { 0.5f,  0.5f, -0.5f, 1.0f, 0.0f },
        { 0.5f, -0.5f, -0.5f, 0.0f, 0.0f },

        { -0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
        { -0.5f,  0.5f,  0.5,  1.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 1.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f }
    };

    const uint32_t indices[36] = 
    { 
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20,
    };

    const uint32_t imageWidth = 2;
    const uint32_t imageHeight = 2;
    const uint32_t imageData[] = { 0xffff0000, 0xff00ff00, 0xff00ffff, 0xff0000ff };

    class PerDrawCb
    {
    public:
        glm::mat4 mWvp;
    };

    class Application
    {
    public:
        Application() = default;
        ~Application() = default;

        void Startup(HINSTANCE instance, int cmdShow)
        {
            // Intialize window and Vulkan
            mWindow.Create(instance, cmdShow, Window::WindowDesc());
            mRenderContext.Init(mWindow.GetHandle(), mWindow.GetWidth(), mWindow.GetHeight());

            // Device object initialization
            mVertexShader.CreateFromFile(mRenderContext.GetDevice(), "SimpleVert.spv");
            mFragmentShader.CreateFromFile(mRenderContext.GetDevice(), "SimpleFrag.spv");

            // Create shader resources
            mSampler.Create(mRenderContext.GetDevice());

            // Create shader resource descriptors
            DescriptorSetLayout descriptorSetLayout;
            VkDescriptorSetLayoutBinding layoutBinding[3] = {};
            layoutBinding[0].binding = 0;
            layoutBinding[0].descriptorCount = 1;
            layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            layoutBinding[0].pImmutableSamplers = nullptr;
            layoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            layoutBinding[1].binding = 1;
            layoutBinding[1].descriptorCount = 1;
            layoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            layoutBinding[1].pImmutableSamplers = nullptr;
            layoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            layoutBinding[2].binding = 2;
            layoutBinding[2].descriptorCount = 1;
            layoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            layoutBinding[2].pImmutableSamplers = mSampler.GetSamplerPtr();
            layoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            descriptorSetLayout.Create(mRenderContext.GetDevice(), layoutBinding, sizeof(layoutBinding) / sizeof(layoutBinding[0]));

            // Create pipeline
            mPipelineLayout.Create(mRenderContext.GetDevice(), descriptorSetLayout);
            mPipeline.Create(mRenderContext.GetDevice(), mRenderContext.GetRenderPass(), mPipelineLayout.GetPipelineLayout(), mVertexShader.GetShaderModule(), mFragmentShader.GetShaderModule());

            vkResetFences(mRenderContext.GetDevice().GetDevice(), 1, &mRenderContext.GetFence(0));

            VkCommandBufferBeginInfo cbBeginInfo = {};
            cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            // Load sample assets
            ObjMesh objMesh;
            objMesh.CreateFromFile("box.obj");

            // Create buffers for mesh and texture and upload their respective data
            vkBeginCommandBuffer(mRenderContext.GetUploadCommandBuffer().GetCommandBuffer(), &cbBeginInfo);
            
            mVertexBuffer.CreateMeshBuffer(
                mRenderContext.GetDevice(), 
                mRenderContext.GetAllocator(), 
                mRenderContext.GetUploadCommandBuffer(), 
                reinterpret_cast<const uint8_t*>(cubeVertices), 
                sizeof(cubeVertices), 
                Buffer::BufferType::Vertex);

            mIndexBuffer.CreateMeshBuffer(
                mRenderContext.GetDevice(), 
                mRenderContext.GetAllocator(), 
                mRenderContext.GetUploadCommandBuffer(), 
                reinterpret_cast<const uint8_t*>(indices), 
                sizeof(indices), 
                Buffer::BufferType::Index);

            mImage.Create2dImage(
                mRenderContext.GetDevice(),
                mRenderContext.GetAllocator(),
                mRenderContext.GetUploadCommandBuffer(),
                imageWidth,
                imageHeight,
                1,
                VK_FORMAT_R8G8B8A8_UNORM,
                0,
                reinterpret_cast<const uint8_t*>(imageData),
                sizeof(imageData));

            vkEndCommandBuffer(mRenderContext.GetUploadCommandBuffer().GetCommandBuffer());

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = mRenderContext.GetUploadCommandBuffer().GetCommandBufferPtr();
            vkQueueSubmit(mRenderContext.GetDevice().GetQueue(), 1, &submitInfo, mRenderContext.GetFence(0));

            vkWaitForFences(mRenderContext.GetDevice().GetDevice(), 1, &mRenderContext.GetFence(0), VK_TRUE, UINT64_MAX);

            mDescriptorPool.Create(mRenderContext.GetDevice());
            mDescriptorSet.Create(mRenderContext.GetDevice(), mDescriptorPool, descriptorSetLayout);

            mUniformBuffer.CreateConstantBuffer(mRenderContext.GetDevice(), mRenderContext.GetAllocator(), sizeof(PerDrawCb));
            PerDrawCb constantBufferData;
            constantBufferData.mWvp = glm::mat4(1.0f);
            mUniformBuffer.UpdateConstantBuffer(mRenderContext.GetDevice(), reinterpret_cast<uint8_t*>(&constantBufferData.mWvp), sizeof(constantBufferData));
            mDescriptorSet.Update(mRenderContext.GetDevice(), mUniformBuffer, mImage, mSampler);
        }

        void Shutdown()
        {

        }

        void CalcModelToProjection(glm::mat4& outModelToProjection)
        {
            static float t = 0.0f;
            t += 0.001f;
            if (t > 1.0f)
            {
                t -= 1.0f;
            }
            glm::mat4 projection = glm::perspective(glm::radians(85.0f), static_cast<float>(mWindow.GetWidth()) / static_cast<float>(mWindow.GetHeight()), 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
            glm::mat4 model = glm::rotate(glm::mat4(1.0), t * 2.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
            outModelToProjection = projection * view * model;
        }

        void Mainloop()
        {
            PerDrawCb constantBufferData;
            CalcModelToProjection(constantBufferData.mWvp);
            mUniformBuffer.UpdateConstantBuffer(mRenderContext.GetDevice(), reinterpret_cast<uint8_t*>(&constantBufferData.mWvp), sizeof(constantBufferData));

            mRenderContext.BeginPass(mWindow.GetWidth(), mWindow.GetHeight());

            CommandBuffer& curCommandBuffer = mRenderContext.GetCurrentCommandBuffer();

            VkViewport viewports[1] = {};
            viewports[0].width = static_cast<float>(mWindow.GetWidth());
            viewports[0].height = static_cast<float>(mWindow.GetHeight());
            viewports[0].minDepth = 0;
            viewports[0].maxDepth = 1;
            vkCmdSetViewport(curCommandBuffer.GetCommandBuffer(), 0, 1, viewports);

            VkRect2D scissors[1] = {};
            scissors[0].extent.width = mWindow.GetWidth();
            scissors[0].extent.height = mWindow.GetHeight();
            vkCmdSetScissor(curCommandBuffer.GetCommandBuffer(), 0, 1, scissors);

            vkCmdBindPipeline(curCommandBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline.GetPipeline());
            VkDeviceSize offset = 0;
            vkCmdBindIndexBuffer(curCommandBuffer.GetCommandBuffer(), mIndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindVertexBuffers(curCommandBuffer.GetCommandBuffer(), 0, 1, mVertexBuffer.GetBufferPtr(), &offset);
            vkCmdBindDescriptorSets(curCommandBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout.GetPipelineLayout(), 0, 1, mDescriptorSet.GetDescriptorSetPtr(), 0, nullptr);
            vkCmdDrawIndexed(curCommandBuffer.GetCommandBuffer(), 36, 1, 0, 0, 0);

            mRenderContext.EndPass();
        }

    private:
        Window         mWindow;
        RenderContext  mRenderContext;
        Shader         mVertexShader;
        Shader         mFragmentShader;
        Pipeline       mPipeline;
        PipelineLayout mPipelineLayout;
        Buffer         mVertexBuffer;
        Buffer         mIndexBuffer;
        DescriptorSet  mDescriptorSet;
        DescriptorPool mDescriptorPool;
        Buffer         mUniformBuffer;
        Image          mImage;
        Sampler        mSampler;
    };
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    int crtDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    crtDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(crtDbgFlag);

    Vnm::Application application;
    application.Startup(hInstance, nCmdShow);

    MSG message = {};
    while (1)
    {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        if (message.message == WM_QUIT)
        {
            break;
        }

        // Processing
        application.Mainloop();
    }

    application.Shutdown();
    return static_cast<int>(message.wParam);
}
