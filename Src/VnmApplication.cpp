// VnmApplication.cpp

#include "VnmApplication.h"

namespace Vnm
{
    void Application::Create(HINSTANCE instance, int cmdShow)
    {
        // Intialize window and Vulkan
        mWindow.Create(instance, cmdShow, Window::WindowDesc());
        mRenderContext.Init(mWindow.GetHandle(), mWindow.GetWidth(), mWindow.GetHeight());
    }
}
