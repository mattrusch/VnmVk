// Window.h

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

LRESULT CALLBACK DefaultWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

namespace Vnm
{
    class MouseState
    {
    public:
        MouseState() = default;
        ~MouseState() = default;

        bool mLeftButtonDown   = false;
        bool mMiddleButtonDown = false;
        bool mRightButtonDown  = false;
        int  mMouseX           = 0;
        int  mMouseY           = 0;
    };

    class InputState
    {
    public:
        InputState() = default;
        ~InputState() = default;

        bool mForward = false;
        bool mReverse = false;
        bool mRight   = false;
        bool mLeft    = false;
    };

    class Window
    {
    public:
        Window() = default;
        ~Window() = default;

        class WindowDesc
        {
        public:
            int      mTop     = 100;
            int      mLeft    = 100;
            int      mWidth   = 1920;
            int      mHeight  = 1200;
            LPCWSTR  mpTitle  = L"Vnm Window";
            WNDPROC  mWndProc = DefaultWndProc;
        };

        void Create(
            HINSTANCE instance, 
            int cmdShow, 
            const WindowDesc& desc);

        void Destroy();

        HWND GetHandle() const      { return mHandle; }
        int  GetWidth() const       { return mWidth; }
        int  GetHeight() const      { return mHeight; }

        MouseState& GetMouseState() { return mMouseState; }
        InputState& GetInputState() { return mInputState; }

    private:
        MouseState mMouseState;
        InputState mInputState;
        HWND       mHandle;
        int        mWidth;
        int        mHeight;
    };
}
