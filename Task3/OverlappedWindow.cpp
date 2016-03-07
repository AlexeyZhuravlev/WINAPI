// Автор: Алексей Журавлев
// Описание: Реализация методов класса COverlappedWindow. Описание класса в файле "OverlappedWindow.h"

#include "OverlappedWindow.h"
#include "EllipseWindow.h"
#include <Windows.h>
#include <ctime>

COverlappedWindow::COverlappedWindow()
{
    handle = 0;
    for( unsigned int i = 0; i < childWindows.size(); i++ ) {
        childWindows[i] = CEllipseWindow();
    }
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
    CreateWindowEx(0, L"OverlappedWindow", L"Parent Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
    return (handle != 0);
}

void COverlappedWindow::Show(int cmdShow)
{
    ShowWindow(handle, cmdShow);
    for (unsigned int i = 0; i < childWindows.size(); i++) {
        childWindows[i].Show(cmdShow);
    }
}

void COverlappedWindow::OnNCCreate(HWND _handle) 
{
    handle = _handle;
}

void COverlappedWindow::OnCreate()
{
    CEllipseWindow::RegisterClass();
    for( int i = 0; i < childWindows.size(); i++ ){
        childWindows[i].Create(handle);
    }
}

void COverlappedWindow::OnSize()
{
    RECT rect;
    int middleX, middleY, nWidth, nHeight;
    ::GetClientRect(handle, &rect);
    middleX = (rect.left + rect.right) / 2;
    middleY = (rect.top + rect.bottom) / 2;
    nWidth = (rect.right - rect.left) / 2;
    nHeight = (rect.bottom - rect.top) / 2;
    SetWindowPos(childWindows[0].getHandle(), HWND_TOP, rect.left, rect.top, nWidth, nHeight, 0);
    SetWindowPos(childWindows[1].getHandle(), HWND_TOP, middleX, rect.top, nWidth, nHeight, 0);
    SetWindowPos(childWindows[2].getHandle(), HWND_TOP, rect.left, middleY, nWidth, nHeight, 0);
    SetWindowPos(childWindows[3].getHandle(), HWND_TOP, middleX, middleY, nWidth, nHeight, 0);
}

void COverlappedWindow::OnDestroy() 
{
}

// Оконная процедура. Обработка сообщений, приходящих в окно.
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
        window->OnNCCreate(handle);
        return DefWindowProc(handle, message, wParam, lParam);
    }
    case WM_CREATE:
    {
        COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
        window->OnCreate();
        return DefWindowProc(handle, message, wParam, lParam);
    }
    case WM_SIZE:
    {
        COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
        window->OnSize();
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