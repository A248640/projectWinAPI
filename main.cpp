// D:/gcc/bin/g++.exe -mwindows -O0 -g -std=c++20 -o build/main.exe source/main.cpp -I header

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <math.h>

void drawGrid(HDC dc, int width, int height, int step)
{
    for(int i { step - 1 }; i < width; i += step)
    {
        MoveToEx(dc, i, 0, nullptr);
        LineTo  (dc, i, height);
    }

    for(int i { step - 1 }; i < height; i += step)
    {
        MoveToEx(dc, 0, i, nullptr);
        LineTo  (dc, width, i);
    }
}

LRESULT CALLBACK windowMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int mouseX { }, mouseY { };
    static int windowWidth { }, windowHeight { };

    if(message == WM_PAINT)
    {
        PAINTSTRUCT ps { };
        HDC windowDC = BeginPaint(window, &ps);
        {
            HDC     bufferDC     { CreateCompatibleDC(windowDC) };
            HBITMAP bufferBitmap { CreateCompatibleBitmap(windowDC, windowWidth, windowHeight) };
            
            HBITMAP windowBitmap { (HBITMAP)SelectObject(bufferDC, bufferBitmap) };
            {
                // кисти по-умолчанию
                HBRUSH dcPen   { (HBRUSH)GetStockObject(DC_PEN)   };
                HBRUSH dcBrush { (HBRUSH)GetStockObject(DC_BRUSH) };

                SelectObject(bufferDC, dcPen);
                SelectObject(bufferDC, dcBrush);

                // очистить экран
                SetDCPenColor  (bufferDC, RGB(255, 255,   0));
                SetDCBrushColor(bufferDC, RGB(100, 149, 237));
                FillRect(bufferDC, &ps.rcPaint, dcBrush);

                // нарисовать сетку
                SetDCPenColor(bufferDC, RGB(64, 64, 64));
                drawGrid(bufferDC, windowWidth, windowHeight, 20);

                // нарисовать квадратик
                SetDCPenColor  (bufferDC, RGB(255, 255, 0));
                SetDCBrushColor(bufferDC, RGB(255, 0,   0));
                Rectangle(bufferDC, mouseX - 5, mouseY - 5, mouseX + 4, mouseY + 4);
            
                // копировать буфер на экран
                BitBlt(windowDC, 0, 0, windowWidth, windowHeight, bufferDC, 0, 0, SRCCOPY);
            }
            SelectObject(bufferDC, windowBitmap);

            DeleteObject(bufferBitmap);
            DeleteDC(bufferDC);
        }
        EndPaint(window, &ps);

        return 0;
    }
    else
    if(message == WM_MOUSEMOVE)
    {
        mouseX = GET_X_LPARAM(lParam); 
        mouseY = GET_Y_LPARAM(lParam); 
        
        return 0;
    }
    else
    if(message == WM_SIZE)
    {
        windowWidth  = LOWORD(lParam);
        windowHeight = HIWORD(lParam);
        
        return 0;
    }
    else
    if(message == WM_ERASEBKGND)
    {
        return 1;
    }
    else
    if(message == WM_DESTROY)
    {
        PostQuitMessage(0);

        return 0;
    }
    else
    {
        return DefWindowProcA(window, message, wParam, lParam);
    }
}

int main()
{
    const char* windowTitle     { "Grid" };
    const char* windowClassName { "window" };
    int windowWidth  { 800 };
    int windowHeight { 600 };

    WNDCLASSEXA windowClass { };
    windowClass.cbSize        = sizeof(windowClass);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = windowMessageHandler;
    // windowClass.cbClsExtra    = 0;
    // windowClass.cbWndExtra    = 0;
    // windowClass.hInstance     = 0;
    windowClass.hIcon         = (HICON)  LoadImageA(0, IDI_APPLICATION, IMAGE_ICON  , 0, 0, LR_SHARED);
    windowClass.hCursor       = (HCURSOR)LoadImageA(0, IDC_ARROW      , IMAGE_CURSOR, 0, 0, LR_SHARED);
    windowClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    // windowClass.lpszMenuName  = 0;
    windowClass.lpszClassName = windowClassName;
    windowClass.hIconSm       = (HICON)  LoadImageA(0, IDI_APPLICATION, IMAGE_ICON  , 0, 0, LR_SHARED);

    if(RegisterClassExA(&windowClass) == 0)
    {
        MessageBoxA(0, "Error in stage 1", "Error", MB_ICONERROR);

        return 0;
    }

    int windowX { (GetSystemMetrics(SM_CXSCREEN) -  windowWidth) / 2 };
    int windowY { (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2 };
    DWORD windowStyle { WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX };

    RECT windowRect = {0, 0, windowWidth, windowHeight };
    AdjustWindowRect(&windowRect, windowStyle, false);

    HWND windowHandle { CreateWindowExA(0, windowClassName, windowTitle, windowStyle, windowX, windowY, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, 0, 0, 0, 0) };
    if(windowHandle == 0)
    {
        MessageBoxA(0, "Error in stage 2", "Error", MB_ICONERROR);

        return 0;
    }

    ShowWindow(windowHandle, SW_SHOW);

    MSG message { };

    while(true)
    {
        if(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            if(message.message == WM_QUIT)
            {
                break;
            }

            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        else
        {
            InvalidateRect(windowHandle, nullptr, false);

            Sleep(1000 / 1000);
        }
    }

    return 0;
}
