// VnmAppRenderVnmMesh.h

#pragma once

#include "VnmApplication.h"
#include "VnmShader.h"
#include "VnmPipeline.h"
#include "VnmPipelineLayout.h"
#include "VnmDescriptorSet.h"
#include "VnmDescriptorPool.h"
#include "VnmMesh.h"

namespace Vnm
{
    class AppRenderVnmMesh : public Application
    {
    public:
        virtual void Startup();
        virtual void Mainloop();
        virtual void Shutdown();

    private:
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
        VnmMesh        mMesh;
    };
}
