// VnmAppRenderObj.cpp

#include "VnmAppRenderObj.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace Vnm
{
    const uint32_t imageWidth = 2;
    const uint32_t imageHeight = 2;
    const uint32_t imageData[] = { 0xffff7777, 0xff00ff00, 0xff00ffff, 0xff0000ff };

    class PerDrawCb
    {
    public:
        glm::mat4 mWorldViewProj;
        glm::mat4 mWorld;
    };

    void AppRenderObj::Startup()
    {
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

        VertexDescription vertexDescription;
        vertexDescription.AddInputBinding({ 0, sizeof(float) * 3 * 3, VK_VERTEX_INPUT_RATE_VERTEX });
        vertexDescription.AddInputAttribute({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 * 0 });
        vertexDescription.AddInputAttribute({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 * 1 });
        vertexDescription.AddInputAttribute({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 * 2 });

        mPipeline.Create(mRenderContext.GetDevice(), vertexDescription, mRenderContext.GetRenderPass(), mPipelineLayout.GetPipelineLayout(), mVertexShader.GetShaderModule(), mFragmentShader.GetShaderModule());

        vkResetFences(mRenderContext.GetDevice().GetDevice(), 1, &mRenderContext.GetFence(0));

        VkCommandBufferBeginInfo cbBeginInfo = {};
        cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // Load sample assets
        mObjMesh.CreateFromFile("decid_tree.obj");

        // Create buffers for mesh and texture and upload their respective data
        vkBeginCommandBuffer(mRenderContext.GetUploadCommandBuffer().GetCommandBuffer(), &cbBeginInfo);

        mVertexBuffer.CreateMeshBuffer(
            mRenderContext.GetDevice(),
            mRenderContext.GetAllocator(),
            mRenderContext.GetUploadCommandBuffer(),
            reinterpret_cast<const uint8_t*>(mObjMesh.GetVertexData()),
            mObjMesh.GetVertexDataSize(),
            Buffer::BufferType::Vertex);

        mIndexBuffer.CreateMeshBuffer(
            mRenderContext.GetDevice(),
            mRenderContext.GetAllocator(),
            mRenderContext.GetUploadCommandBuffer(),
            reinterpret_cast<const uint8_t*>(mObjMesh.GetIndexData()),
            mObjMesh.GetIndexDataSize(),
            Buffer::BufferType::Index);

        const void* mipData = (const void*)imageData;
        size_t mipSize = sizeof(imageData);
        mImage.Create2dImage(
            mRenderContext.GetDevice(),
            mRenderContext.GetAllocator(),
            mRenderContext.GetUploadCommandBuffer(),
            imageWidth,
            imageHeight,
            1,
            VK_FORMAT_R8G8B8A8_UNORM,
            0,
            reinterpret_cast<const uint8_t**>(&mipData),
            &mipSize);

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
        constantBufferData.mWorldViewProj = glm::mat4(1.0f);
        mUniformBuffer.UpdateConstantBuffer(mRenderContext.GetDevice(), reinterpret_cast<uint8_t*>(&constantBufferData.mWorldViewProj), sizeof(constantBufferData));
        mDescriptorSet.Update(mRenderContext.GetDevice(), mUniformBuffer, mImage, mSampler);
    }

    static void CalcModelToProjection(float aspect, glm::mat4& outModelToProjection, glm::mat4& outModelToWorld)
    {
        static float t = 0.0f;
        t += 0.001f;
        if (t > 1.0f)
        {
            t -= 1.0f;
        }
        glm::mat4 projection = glm::perspective(glm::radians(85.0f), aspect, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(15.0f, 15.0f, 15.0f), glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        glm::mat4 model = glm::rotate(glm::mat4(1.0), t * 2.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
        outModelToProjection = projection * view * model;
        outModelToWorld = model;
    }

    void AppRenderObj::Mainloop()
    {
        PerDrawCb constantBufferData;
        CalcModelToProjection(static_cast<float>(mWindow.GetWidth()) / static_cast<float>(mWindow.GetHeight()), constantBufferData.mWorldViewProj, constantBufferData.mWorld);
        mUniformBuffer.UpdateConstantBuffer(mRenderContext.GetDevice(), reinterpret_cast<uint8_t*>(&constantBufferData.mWorldViewProj), sizeof(constantBufferData));

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
        vkCmdDrawIndexed(curCommandBuffer.GetCommandBuffer(), static_cast<uint32_t>(mObjMesh.GetNumIndices()), 1, 0, 0, 0);

        mRenderContext.EndPass();
    }

    void AppRenderObj::Shutdown()
    {}
}
