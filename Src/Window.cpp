// Window.cpp

#include "Window.h"

static inline bool RangeContaninsValue(int value, int start, int end)
{
    return(value >= start && value <= end);
}

LRESULT CALLBACK DefaultWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    Vnm::Window* window = (Vnm::Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (message)
    {
    case WM_NCCREATE:
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)LPCREATESTRUCT(lparam)->lpCreateParams);
        break;
    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE)
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
    {
        Vnm::MouseState mouseState = window->GetMouseState();
        mouseState.mLeftButtonDown = true;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        window->SetMouseState(mouseState);
        break;
    }
    case WM_LBUTTONUP:
    {
        Vnm::MouseState mouseState = window->GetMouseState();
        mouseState.mLeftButtonDown = false;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        window->SetMouseState(mouseState);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        Vnm::MouseState mouseState = window->GetMouseState();
        mouseState.mMiddleButtonDown = true;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        window->SetMouseState(mouseState);
        break;
    }
    case WM_MBUTTONUP:
    {
        Vnm::MouseState mouseState = window->GetMouseState();
        mouseState.mMiddleButtonDown = false;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        window->SetMouseState(mouseState);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        Vnm::MouseState mouseState = window->GetMouseState();
        mouseState.mRightButtonDown = true;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        window->SetMouseState(mouseState);
        break;
    }
    case WM_RBUTTONUP:
    {
        Vnm::MouseState mouseState = window->GetMouseState();
        mouseState.mRightButtonDown = false;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        window->SetMouseState(mouseState);
        break;
    }
    case WM_MOUSEMOVE:
    {
        Vnm::MouseState mouseState = window->GetMouseState();
        SetCapture(hwnd);
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        if (!RangeContaninsValue(mouseState.mMouseX, 0, window->GetWidth()) || !RangeContaninsValue(mouseState.mMouseY, 0, window->GetHeight()))
        {
            mouseState.mLeftButtonDown = false;
            mouseState.mMiddleButtonDown = false;
            mouseState.mRightButtonDown = false;
            ReleaseCapture();
        }
        window->SetMouseState(mouseState);
        break;
    }
    default:
        break;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

namespace Vnm
{
    void Window::Create(HINSTANCE instance, int cmdShow, const WindowDesc& desc)
    {
        WNDCLASSEX wndClass = {0};
        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wndClass.hInstance = instance;
        wndClass.lpfnWndProc = desc.mWndProc;
        wndClass.lpszClassName = desc.mpTitle;
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

        RegisterClassEx(&wndClass);

        // Adjust window so that client size matches what was requested
        DWORD windowStyle = WS_OVERLAPPEDWINDOW;
        ::RECT rect;
        ::SetRect(&rect, 0, 0, desc.mWidth, desc.mHeight);
        ::AdjustWindowRect(&rect, windowStyle, FALSE);

        mHandle = CreateWindow(
            desc.mpTitle, 
            desc.mpTitle, 
            windowStyle,
            CW_USEDEFAULT, 
            CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top, 
            NULL, 
            NULL, 
            instance,
            (void*)this);

        DWORD error = GetLastError();

        ShowWindow(mHandle, cmdShow);

        mWidth = desc.mWidth;
        mHeight = desc.mHeight;
    }

    void Window::Destroy()
    {
        DestroyWindow(mHandle);
    }
}
