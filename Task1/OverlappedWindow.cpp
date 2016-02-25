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
    handle = CreateWindowEx(WS_EX_TOPMOST, L"OverlappedWindow", L"My Window", WS_EX_OVERLAPPEDWINDOW | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
    return (handle != 0);
}

void COverlappedWindow::Show(int cmdShow)
{
    ShowWindow(handle, cmdShow);
}

void COverlappedWindow::OnDestroy()
{
    KillTimer(handle, timerId);
    PostQuitMessage(0);
}

void COverlappedWindow::OnPaint()
{
    PAINTSTRUCT paintStruct;
    RECT rect;
    HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
    HDC paintDC = ::BeginPaint(handle, &paintStruct);
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    SelectObject(paintDC, pen);
    ::GetClientRect(handle, &rect);
    FillRect(paintDC, &rect, brush);
    int r = 30;
    int R = min((rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2) - r;
    int x = (rect.left + rect.right) / 2 + R * cos(time);
    int y = (rect.top + rect.bottom) / 2 + R * sin(time);
    int left = x - r;
    int right = x + r;
    int top = y - r;
    int bottom = y + r;
    HBRUSH innBrush = CreateSolidBrush(RGB(0, 0, 255));
    SelectObject(paintStruct.hdc, innBrush);
    Ellipse(paintDC, left, top, right, bottom);
    DeleteObject(pen);
    DeleteObject(brush);
    DeleteObject(innBrush);
    ::EndPaint(handle, &paintStruct);
}

void COverlappedWindow::OnTimer()
{
    RECT rect;
    time += 0.01;
    ::GetClientRect(handle, &rect);
    InvalidateRect(handle, &rect, FALSE);
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
            window->timerId = SetTimer(handle, NULL, 0.05, NULL);
            window->time = 0;
            return DefWindowProc(handle, message, wParam, lParam);
        }
        case WM_DESTROY:
        {
            COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnDestroy();
            return 0;
        }
        case WM_PAINT:
        {
            COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnPaint();
            return 0;
        }
        case WM_TIMER:
        {
            COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnTimer();
            return 0;
        }
        default:
            return DefWindowProc(handle, message, wParam, lParam);
    }
}