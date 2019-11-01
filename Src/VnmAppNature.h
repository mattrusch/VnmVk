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
    class AppNature : public Application
    {
    public:
        virtual void Startup();
        virtual void Mainloop();
        virtual void Shutdown();

    private:
        Shader            mVertexShader;
        Shader            mFragmentShader;
        Pipeline          mPipeline;
        PipelineLayout    mPipelineLayout;
        Buffer            mVertexBuffer;
        Buffer            mIndexBuffer;
        DescriptorSet     mDescriptorSet[2];
        DescriptorPool    mDescriptorPool;
        Buffer            mUniformBuffer;
        Image             mImage[2];
        Sampler           mSampler;
        VnmMesh           mMesh;
        ThirdPersonCamera mCamera;
    };
}
