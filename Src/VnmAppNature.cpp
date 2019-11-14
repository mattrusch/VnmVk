// VnmAppNature.cpp

// VnmAppRenderVnmMesh.cpp

#include "VnmAppNature.h"
#include "VnmFile.h"
#include "VnmTgaImage.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

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

    static const uint32_t tgaFileBpp = 4;

    // Currently assumes file is a 4 byte per pixel, square power of 2
    static void CreateImageFromFile(Image& dstImage, const char* filename, RenderContext& renderContext)
    {
        FileResource* tgaFileResource = FileResource::LoadFileResource(filename);
        TgaImage tgaImage;
        tgaImage.ParseData(tgaFileResource->GetData());
        assert(tgaImage.GetWidth() == tgaImage.GetHeight());
        assert(tgaImage.GetBytesPerPixel() == 4);

        // Generate mipmaps
        // TODO: Improve or remove and only support pre-generated mips
        std::vector<uint8_t*> mipData;
        std::vector<size_t> mipSize;

        mipData.emplace_back(new uint8_t[tgaImage.GetSize()]);
        memcpy(mipData[0], tgaImage.GetImageData(), tgaImage.GetSize());
        mipSize.emplace_back(tgaImage.GetSize());

        int curWidth = tgaImage.GetWidth();
        int curHeight = tgaImage.GetHeight();
        const uint8_t* prevMipData = tgaImage.GetImageData();

        while (curWidth > 1)
        {
            curWidth >>= 1;
            curHeight >>= 1;

            size_t curMipSize = curWidth * curHeight * tgaFileBpp;
            uint8_t* curMipData = new uint8_t[curMipSize];
            mipData.emplace_back(curMipData);
            mipSize.emplace_back(curMipSize);

            for (int j = 0; j < curHeight; ++j)
            {
                for (int i = 0; i < curWidth; ++i)
                {
                    int srcIndex = (i * 2 + j * 2 * curWidth * 2) * tgaFileBpp;
                    int dstIndex = (i + j * curWidth) * tgaFileBpp;

                    curMipData[dstIndex + 0] = prevMipData[srcIndex + 0];
                    curMipData[dstIndex + 1] = prevMipData[srcIndex + 1];
                    curMipData[dstIndex + 2] = prevMipData[srcIndex + 2];
                    curMipData[dstIndex + 3] = prevMipData[srcIndex + 3];
                }
            }

            prevMipData = curMipData;
        }

        VkCommandBufferBeginInfo cbBeginInfo = {};
        cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(renderContext.GetUploadCommandBuffer().GetCommandBuffer(), &cbBeginInfo);

        dstImage.Create2dImage(
            renderContext.GetDevice(),
            renderContext.GetAllocator(),
            renderContext.GetUploadCommandBuffer(),
            tgaImage.GetWidth(),
            tgaImage.GetHeight(),
            static_cast<int>(mipData.size()),
            VK_FORMAT_B8G8R8A8_UNORM,
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

        for(auto data : mipData)
        {
            delete[] data;
        }

        FileResource::DestroyFileResource(tgaFileResource);
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
            VK_FORMAT_B8G8R8A8_UNORM,
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
        vertexDescription.AddInputBinding({ 0, sizeof(float) * 12, VK_VERTEX_INPUT_RATE_VERTEX });
        vertexDescription.AddInputAttribute({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 });                    // Position
        vertexDescription.AddInputAttribute({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 });    // Normal
        vertexDescription.AddInputAttribute({ 2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 6 }); // Tangent (w = handedness)
        vertexDescription.AddInputAttribute({ 3, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 10 });      // Uv

        mPipeline.Create(mRenderContext.GetDevice(), vertexDescription, mRenderContext.GetRenderPass(), mPipelineLayout.GetPipelineLayout(), mVertexShader.GetShaderModule(), mFragmentShader.GetShaderModule());

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

        const char* mipFilenames[] = 
        {
            "grass_mip0.tga",
            "grass_mip1.tga",
            "grass_mip2.tga",
            "grass_mip3.tga",
            "grass_mip4.tga",
            "grass_mip5.tga",
            "grass_mip6.tga",
            "grass_mip7.tga",
            "grass_mip8.tga",
            "grass_mip9.tga",
            "grass_mip10.tga",
            "grass_mip11.tga"
        };

        int numMips = sizeof(mipFilenames) / sizeof(mipFilenames[0]);

        CreateMippedImageFromFiles(mImage[0], numMips, mipFilenames, mRenderContext);
        CreateImageFromFile(mImage[1], "bad_water.tga", mRenderContext);

        mDescriptorPool.Create(mRenderContext.GetDevice());
        mDescriptorSet[0].Create(mRenderContext.GetDevice(), mDescriptorPool, descriptorSetLayout);
        mDescriptorSet[1].Create(mRenderContext.GetDevice(), mDescriptorPool, descriptorSetLayout);

        mUniformBuffer.CreateConstantBuffer(mRenderContext.GetDevice(), mRenderContext.GetAllocator(), sizeof(PerDrawCb));
        PerDrawCb constantBufferData;
        constantBufferData.mWorldViewProj = glm::mat4(1.0f);
        mUniformBuffer.UpdateConstantBuffer(mRenderContext.GetDevice(), reinterpret_cast<uint8_t*>(&constantBufferData.mWorldViewProj), sizeof(constantBufferData));
        mDescriptorSet[0].Update(mRenderContext.GetDevice(), mUniformBuffer, mImage[0], mSampler);
        mDescriptorSet[1].Update(mRenderContext.GetDevice(), mUniformBuffer, mImage[1], mSampler);
    }

    static void CalcModelToProjection(float aspect, const ThirdPersonCamera& camera, glm::mat4& outModelToProjection, glm::mat4& outModelToWorld)
    {
        static float t = 0.0f;
        t += 0.001f;
        if (t > 1.0f)
        {
            t -= 1.0f;
        }
        glm::mat4 projection = glm::perspective(glm::radians(55.0f), aspect, 0.1f, 100.0f);
        glm::mat4 view = camera.GetLookAt();
        glm::mat4 model = glm::mat4(1.0f);
        outModelToProjection = projection * view * model;
        outModelToWorld = model;
    }

    static void UpdateCamera(const MouseState& mouseState, ThirdPersonCamera& camera)
    {
        static int prevX = mouseState.mMouseX;
        static int prevY = mouseState.mMouseY;

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
        UpdateCamera(mWindow.GetMouseState(), mCamera);

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

        vkCmdBindPipeline(curCommandBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline.GetPipeline());
        VkDeviceSize offset = 0;
        vkCmdBindIndexBuffer(curCommandBuffer.GetCommandBuffer(), mIndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindVertexBuffers(curCommandBuffer.GetCommandBuffer(), 0, 1, mVertexBuffer.GetBufferPtr(), &offset);

        uint32_t vertexOffset = 0;
        int32_t indexOffset = 0;
        for (size_t i = 0, size = mMesh.GetNumSubmeshes(); i < size; ++i)
        {
            vkCmdBindDescriptorSets(curCommandBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout.GetPipelineLayout(), 0, 1, mDescriptorSet[i].GetDescriptorSetPtr(), 0, nullptr);

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
