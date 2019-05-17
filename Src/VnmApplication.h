// VnmApplication.h

#pragma once

#include "Window.h"
#include "VnmRenderContext.h"

namespace Vnm
{
    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

        void Create(HINSTANCE instance, int cmdShow);

        virtual void Startup() = 0;
        virtual void Mainloop() = 0;
        virtual void Shutdown() = 0;

    protected:
        Window         mWindow;
        RenderContext  mRenderContext;
    };
}
