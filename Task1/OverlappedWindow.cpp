// Автор: Алексей Журавлев
// Описание: Реализация методов класса COverlappedWindow. Описание класса в файле "OverlappedWindow.h"

#include "OverlappedWindow.h"
#include <Windows.h>
#include <iostream>

COverlappedWindow::COverlappedWindow() 
{
}

COverlappedWindow::~COverlappedWindow()
{
}

bool COverlappedWindow::RegisterClass()
{
    WNDCLASSEX windowClass;
    ::ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc = COverlappedWindow::windowProc;
    windowClass.hInstance = GetModuleHandle(0);
    windowClass.lpszClassName = L"OverlappedWindow";
    return (::RegisterClassEx(&windowClass) != 0);
}

bool COverlappedWindow::Create()
{
    handle = CreateWindowEx(WS_EX_TOPMOST, L"OverlappedWindow", L"My Window", WS_EX_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
    return (handle != 0);
}

void COverlappedWindow::Show(int cmdShow)
{
    ShowWindow(handle, cmdShow);
}

void COverlappedWindow::OnDestroy()
{
    PostQuitMessage(0);
}

// Оконная процедура. Обработка сообщений, приходящий в окно.
LRESULT COverlappedWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_NCCREATE:
        {
            COverlappedWindow* window = (COverlappedWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
            SetLastError(0);
            SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)window);
            if( GetLastError() != 0 ) {
                return GetLastError();
            }
            return DefWindowProc(handle, message, wParam, lParam);
        }
        case WM_DESTROY:
        {
            COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnDestroy();
            return 0;
        }
        default:
            return DefWindowProc(handle, message, wParam, lParam);
    }
}