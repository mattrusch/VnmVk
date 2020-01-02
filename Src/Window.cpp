// Window.cpp

#include "Window.h"

static inline bool RangeContaninsValue(int value, int start, int end)
{
    return(value >= start && value <= end);
}

LRESULT CALLBACK DefaultWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    Vnm::Window* window = (Vnm::Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    Vnm::MouseState& mouseState = window->GetMouseState();

    // TODO: Consider moving to RAWINPUT rather than windows messaging for keyboard/mouse input
    switch (message)
    {
    case WM_NCCREATE:
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)LPCREATESTRUCT(lparam)->lpCreateParams);
        break;
    case WM_KEYDOWN:
        switch (wparam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        case 'W':
            window->GetInputState().mForward = true;
            break;
        case 'S':
            window->GetInputState().mReverse = true;
            break;
        case 'A':
            window->GetInputState().mLeft = true;
            break;
        case 'D':
            window->GetInputState().mRight = true;
            break;
        default:
            break;
        }
        break;
    case WM_KEYUP:
        switch (wparam)
        {
        case 'W':
            window->GetInputState().mForward = false;
            break;
        case 'S':
            window->GetInputState().mReverse = false;
            break;
        case 'A':
            window->GetInputState().mLeft = false;
            break;
        case 'D':
            window->GetInputState().mRight = false;
            break;
        default:
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
    {
        mouseState.mLeftButtonDown = true;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        break;
    }
    case WM_LBUTTONUP:
    {
        mouseState.mLeftButtonDown = false;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        mouseState.mMiddleButtonDown = true;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        break;
    }
    case WM_MBUTTONUP:
    {
        mouseState.mMiddleButtonDown = false;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        mouseState.mRightButtonDown = true;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
         break;
    }
    case WM_RBUTTONUP:
    {
         mouseState.mRightButtonDown = false;
        mouseState.mMouseX = LOWORD(lparam);
        mouseState.mMouseY = HIWORD(lparam);
        break;
    }
    case WM_MOUSEMOVE:
    {
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
