// VnmAppNature.h

#pragma once

#include "VnmApplication.h"
#include "VnmShader.h"
#include "VnmPipeline.h"
#include "VnmPipelineLayout.h"
#include "VnmDescriptorSet.h"
#include "VnmDescriptorPool.h"
#include "VnmMesh.h"
#include "VnmCamera.h"

namespace Vnm
{
    enum ShaderType
    {
        Simple,
        NormalMap,
        NumShaderTypes
    };

    enum TextureType
    {
        GrassAlbedo,
        DirtAlbedo,
        DirtNormal,
        BadWater,
        NumTextureTypes
    };

    // These currently must correspond with the submesh indices
    enum MaterialType
    {
        SimpleTerrain,
        SimpleWater,
        NmTerrain,
        NumMaterialTypes
    };

    class AppNature : public Application
    {
    public:
        virtual void Startup();
        virtual void Mainloop();
        virtual void Shutdown();

    private:
        Shader            mVertexShader[NumShaderTypes];
        Shader            mFragmentShader[NumShaderTypes];
        Pipeline          mPipeline[NumShaderTypes];
        PipelineLayout    mPipelineLayout[NumShaderTypes];
        Buffer            mVertexBuffer;
        Buffer            mIndexBuffer;
        DescriptorSet     mDescriptorSets[NumMaterialTypes];
        DescriptorPool    mDescriptorPool;
        Buffer            mUniformBuffer;
        Image             mImage[NumTextureTypes];
        Sampler           mSampler;
        VnmMesh           mMesh;
        ThirdPersonCamera mCamera;
    };
}
