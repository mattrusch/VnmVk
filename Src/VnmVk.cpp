// VnmVk.cpp

#include "Window.h"
#include "crtdbg.h"
#include "VnmAppRenderObj.h"
#include "VnmAppRenderVnmMesh.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    int crtDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    crtDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(crtDbgFlag);

    //Vnm::AppRenderObj application;
    Vnm::AppRenderVnmMesh application;
    application.Create(hInstance, nCmdShow);
    application.Startup();

    MSG message = {};
    while (1)
    {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        if (message.message == WM_QUIT)
        {
            break;
        }

        // Processing
        application.Mainloop();
    }

    application.Shutdown();
    return static_cast<int>(message.wParam);
}
