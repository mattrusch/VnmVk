// VnmAppNature.cpp

// VnmAppRenderVnmMesh.cpp

#include "VnmAppNature.h"
#include "VnmFile.h"
#include "VnmTgaImage.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "gli/gli.hpp"

#include <vector>
#include <algorithm>
#include <cassert>

namespace Vnm
{
    class PerDrawCb
    {
    public:
        glm::mat4 mWorldViewProj;
        glm::mat4 mWorld;
    };

    static void CreateImageFromFile(Image& dstImage, const char* filename, RenderContext& renderContext)
    {
        gli::texture2d texture(gli::load(filename));
        assert(!texture.empty());

        std::vector<const uint8_t*> mipData(texture.levels());
        std::vector<size_t> mipSize(texture.levels());

        for (int i = 0; i < texture.levels(); ++i)
        {
            mipData[i] = (const uint8_t*)texture[i].data();
            mipSize[i] = texture[i].size();
        }

        VkCommandBufferBeginInfo cbBeginInfo = {};
        cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(renderContext.GetUploadCommandBuffer().GetCommandBuffer(), &cbBeginInfo);

        dstImage.Create2dImage(
            renderContext.GetDevice(),
            renderContext.GetAllocator(),
            renderContext.GetUploadCommandBuffer(),
            texture.extent().x,
            texture.extent().y,
            static_cast<int>(texture.levels()),
            VK_FORMAT_R8G8B8A8_UNORM, // TODO
            0,
            mipData.data(),
            mipSize.data());

        vkEndCommandBuffer(renderContext.GetUploadCommandBuffer().GetCommandBuffer());

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = renderContext.GetUploadCommandBuffer().GetCommandBufferPtr();
        vkQueueSubmit(renderContext.GetDevice().GetQueue(), 1, &submitInfo, renderContext.GetFence(0));

        vkWaitForFences(renderContext.GetDevice().GetDevice(), 1, &renderContext.GetFence(0), VK_TRUE, UINT64_MAX);
    }

    static void CreateMippedImageFromFiles(Image& dstImage, size_t numMips, const char* const* mipFilenames, RenderContext& renderContext)
    {
        std::vector<FileResource*> fileResources(numMips);
        std::vector<TgaImage> tgaImages(numMips);
        std::vector<const uint8_t*> mipData(numMips);
        std::vector<size_t> mipSize(numMips);

        for (int i = 0; i < numMips; ++i)
        {
            fileResources[i] = FileResource::LoadFileResource(mipFilenames[i]);
            tgaImages[i].ParseData(fileResources[i]->GetData());
            mipData[i] = tgaImages[i].GetImageData();
            mipSize[i] = tgaImages[i].GetSize();
        }

        VkCommandBufferBeginInfo cbBeginInfo = {};
        cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(renderContext.GetUploadCommandBuffer().GetCommandBuffer(), &cbBeginInfo);

        dstImage.Create2dImage(
            renderContext.GetDevice(),
            renderContext.GetAllocator(),
            renderContext.GetUploadCommandBuffer(),
            tgaImages[0].GetWidth(),
            tgaImages[0].GetHeight(),
            static_cast<int>(mipData.size()),
            VK_FORMAT_B8G8R8A8_UNORM, // TODO
            0,
            mipData.data(),
            mipSize.data());

        vkEndCommandBuffer(renderContext.GetUploadCommandBuffer().GetCommandBuffer());

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = renderContext.GetUploadCommandBuffer().GetCommandBufferPtr();
        vkQueueSubmit(renderContext.GetDevice().GetQueue(), 1, &submitInfo, renderContext.GetFence(0));

        vkWaitForFences(renderContext.GetDevice().GetDevice(), 1, &renderContext.GetFence(0), VK_TRUE, UINT64_MAX);

        for (auto resource : fileResources)
        {
            FileResource::DestroyFileResource(resource);
        }
    }

    void AppNature::Startup()
    {
        mCamera.SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
        mCamera.SetTarget(glm::vec3(0.0f, 0.0f, 0.75f));

        // Device object initialization
        mVertexShader[Simple].CreateFromFile(mRenderContext.GetDevice(), "SimpleVert.spv");
        mFragmentShader[Simple].CreateFromFile(mRenderContext.GetDevice(), "SimpleFrag.spv");

        mVertexShader[NormalMap].CreateFromFile(mRenderContext.GetDevice(), "NormalMap.spv");
        mFragmentShader[NormalMap].CreateFromFile(mRenderContext.GetDevice(), "NormalMap.spv");

        // Create shader resources
        mSampler.Create(mRenderContext.GetDevice());

        // Create shader resource descriptors
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
        DescriptorSetLayout simpleDescriptorSetLayout;
        simpleDescriptorSetLayout.Create(mRenderContext.GetDevice(), layoutBinding, sizeof(layoutBinding) / sizeof(layoutBinding[0]));

        // Create pipeline
        mPipelineLayout[Simple].Create(mRenderContext.GetDevice(), simpleDescriptorSetLayout);

        VkDescriptorSetLayoutBinding nmLayoutBinding[5] = {};
        nmLayoutBinding[0].binding = 0;
        nmLayoutBinding[0].descriptorCount = 1;
        nmLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        nmLayoutBinding[0].pImmutableSamplers = nullptr;
        nmLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        nmLayoutBinding[1].binding = 1;
        nmLayoutBinding[1].descriptorCount = 1;
        nmLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        nmLayoutBinding[1].pImmutableSamplers = nullptr;
        nmLayoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        nmLayoutBinding[2].binding = 2;
        nmLayoutBinding[2].descriptorCount = 1;
        nmLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        nmLayoutBinding[2].pImmutableSamplers = mSampler.GetSamplerPtr();
        nmLayoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        nmLayoutBinding[3].binding = 3;
        nmLayoutBinding[3].descriptorCount = 1;
        nmLayoutBinding[3].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        nmLayoutBinding[3].pImmutableSamplers = nullptr;
        nmLayoutBinding[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        nmLayoutBinding[4].binding = 4;
        nmLayoutBinding[4].descriptorCount = 1;
        nmLayoutBinding[4].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        nmLayoutBinding[4].pImmutableSamplers = mSampler.GetSamplerPtr();
        nmLayoutBinding[4].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        DescriptorSetLayout nmDescriptorSetLayout;
        nmDescriptorSetLayout.Create(mRenderContext.GetDevice(), nmLayoutBinding, sizeof(nmLayoutBinding) / sizeof(nmLayoutBinding[0]));

        // Create pipeline
        mPipelineLayout[NormalMap].Create(mRenderContext.GetDevice(), nmDescriptorSetLayout);

        VertexDescription vertexDescription;
        vertexDescription.AddInputBinding({ 0, sizeof(float) * 12, VK_VERTEX_INPUT_RATE_VERTEX });
        vertexDescription.AddInputAttribute({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 });                    // Position
        vertexDescription.AddInputAttribute({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 });    // Normal
        vertexDescription.AddInputAttribute({ 2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 6 }); // Tangent (w = handedness)
        vertexDescription.AddInputAttribute({ 3, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 10 });      // Uv

        mPipeline[Simple].Create(
            mRenderContext.GetDevice(), 
            vertexDescription, 
            mRenderContext.GetRenderPass(), 
            mPipelineLayout[Simple].GetPipelineLayout(), 
            mVertexShader[Simple].GetShaderModule(), 
            mFragmentShader[Simple].GetShaderModule());

        vkResetFences(mRenderContext.GetDevice().GetDevice(), 1, &mRenderContext.GetFence(0));

        VkCommandBufferBeginInfo cbBeginInfo = {};
        cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // Load sample assets
        mMesh.CreateFromFile("rivers_terrain.vnm");

        // Create buffers for mesh and texture and upload their respective data
        vkBeginCommandBuffer(mRenderContext.GetUploadCommandBuffer().GetCommandBuffer(), &cbBeginInfo);

        mVertexBuffer.CreateMeshBuffer(
            mRenderContext.GetDevice(),
            mRenderContext.GetAllocator(),
            mRenderContext.GetUploadCommandBuffer(),
            reinterpret_cast<const uint8_t*>(mMesh.GetVertexData()),
            mMesh.GetVertexDataSize(),
            Buffer::BufferType::Vertex);

        mIndexBuffer.CreateMeshBuffer(
            mRenderContext.GetDevice(),
            mRenderContext.GetAllocator(),
            mRenderContext.GetUploadCommandBuffer(),
            reinterpret_cast<const uint8_t*>(mMesh.GetIndexData()),
            mMesh.GetIndexDataSize(),
            Buffer::BufferType::Index);

        vkEndCommandBuffer(mRenderContext.GetUploadCommandBuffer().GetCommandBuffer());

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = mRenderContext.GetUploadCommandBuffer().GetCommandBufferPtr();
        vkQueueSubmit(mRenderContext.GetDevice().GetQueue(), 1, &submitInfo, mRenderContext.GetFence(0));

        vkWaitForFences(mRenderContext.GetDevice().GetDevice(), 1, &mRenderContext.GetFence(0), VK_TRUE, UINT64_MAX);

        CreateImageFromFile(mImage[GrassAlbedo], "grass_green_01_ARGB_8888_1.KTX", mRenderContext);
        CreateImageFromFile(mImage[DirtAlbedo], "rocky-worn-ground-albedo_ARGB_8888_1.KTX", mRenderContext);
        CreateImageFromFile(mImage[BadWater], "bad_water_ARGB_8888_1.KTX", mRenderContext);

        mDescriptorPool.Create(mRenderContext.GetDevice());
        mDescriptorSets[SimpleTerrain].Create(mRenderContext.GetDevice(), mDescriptorPool, simpleDescriptorSetLayout);
        mDescriptorSets[SimpleWater].Create(mRenderContext.GetDevice(), mDescriptorPool, simpleDescriptorSetLayout);

        mUniformBuffer.CreateConstantBuffer(mRenderContext.GetDevice(), mRenderContext.GetAllocator(), sizeof(PerDrawCb));
        PerDrawCb constantBufferData;
        constantBufferData.mWorldViewProj = glm::mat4(1.0f);
        mUniformBuffer.UpdateConstantBuffer(mRenderContext.GetDevice(), reinterpret_cast<uint8_t*>(&constantBufferData.mWorldViewProj), sizeof(constantBufferData));
        mDescriptorSets[SimpleTerrain].Update(
            mRenderContext.GetDevice(), 
            std::vector<VkDescriptorType> { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE }, 
            std::vector<Buffer*> { &mUniformBuffer },
            std::vector<Image*> { &mImage[DirtAlbedo] },
            std::vector<Sampler*> { &mSampler });
        mDescriptorSets[SimpleWater].Update(
            mRenderContext.GetDevice(),
            std::vector<VkDescriptorType> { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE },
            std::vector<Buffer*> { &mUniformBuffer },
            std::vector<Image*> { &mImage[BadWater] },
            std::vector<Sampler*> { &mSampler });
    }

    static void CalcModelToProjection(float aspect, const ThirdPersonCamera& camera, glm::mat4& outModelToProjection, glm::mat4& outModelToWorld)
    {
        glm::mat4 projection = glm::perspective(glm::radians(55.0f), aspect, 0.1f, 100.0f);
        glm::mat4 view = camera.GetLookAt();
        glm::mat4 model = glm::mat4(1.0f);
        outModelToProjection = projection * view * model;
        outModelToWorld = model;
    }

    static void UpdateCamera(const MouseState& mouseState, const InputState& inputState, ThirdPersonCamera& camera)
    {
        static int prevX = mouseState.mMouseX;
        static int prevY = mouseState.mMouseY;

        const float kMoveSpeedFactor = 0.01f;
        if (inputState.mForward)
        {
            camera.MoveForward(kMoveSpeedFactor);
        }
        else if (inputState.mReverse)
        {
            camera.MoveForward(-kMoveSpeedFactor);
        }

        if (inputState.mLeft)
        {
            camera.MoveRightConstrainHeight(-kMoveSpeedFactor);
        }
        else if (inputState.mRight)
        {
            camera.MoveRightConstrainHeight(kMoveSpeedFactor);
        }

        if (mouseState.mLeftButtonDown)
        {
            const float kRotationSpeedFactor = 0.001f;
            float deltaY = ((float)mouseState.mMouseY - (float)prevY) * kRotationSpeedFactor;
            float deltaX = ((float)mouseState.mMouseX - (float)prevX) * kRotationSpeedFactor;
            camera.Orbit(-deltaY, deltaX);
        }

        prevX = mouseState.mMouseX;
        prevY = mouseState.mMouseY;
    }

    void AppNature::Mainloop()
    {
        UpdateCamera(mWindow.GetMouseState(), mWindow.GetInputState(), mCamera);

        PerDrawCb constantBufferData;
        CalcModelToProjection(static_cast<float>(mWindow.GetWidth()) / static_cast<float>(mWindow.GetHeight()), mCamera, constantBufferData.mWorldViewProj, constantBufferData.mWorld);
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

        vkCmdBindPipeline(curCommandBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline[Simple].GetPipeline());
        VkDeviceSize offset = 0;
        vkCmdBindIndexBuffer(curCommandBuffer.GetCommandBuffer(), mIndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindVertexBuffers(curCommandBuffer.GetCommandBuffer(), 0, 1, mVertexBuffer.GetBufferPtr(), &offset);

        uint32_t vertexOffset = 0;
        int32_t indexOffset = 0;
        for (size_t i = 0, size = mMesh.GetNumSubmeshes(); i < size; ++i)
        {
            vkCmdBindDescriptorSets(curCommandBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout[Simple].GetPipelineLayout(), 0, 1, mDescriptorSets[i].GetDescriptorSetPtr(), 0, nullptr);

            const SubmeshDesc* submeshDesc = mMesh.GetSubmeshData() + i;
            vkCmdDrawIndexed(curCommandBuffer.GetCommandBuffer(), submeshDesc->mNumIndices, 1, indexOffset, vertexOffset, 0);
            indexOffset += submeshDesc->mNumIndices;
            vertexOffset += submeshDesc->mNumVertices;
        }

        mRenderContext.EndPass();
    }

    void AppNature::Shutdown()
    {}
}
