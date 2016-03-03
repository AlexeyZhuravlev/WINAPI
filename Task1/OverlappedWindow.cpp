// Автор: Алексей Журавлев
// Описание: Реализация методов класса COverlappedWindow. Описание класса в файле "OverlappedWindow.h"

#include "OverlappedWindow.h"
#include <Windows.h>
#include <iostream>

const double COverlappedWindow::deltaTime = 0.05;

COverlappedWindow::COverlappedWindow() 
{
    time = 0;
    handle = 0;
    timerId = 0;
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
    windowClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    return (::RegisterClassEx(&windowClass) != 0);
}

bool COverlappedWindow::Create()
{
    CreateWindowEx(0, L"OverlappedWindow", L"My Window", WS_EX_OVERLAPPEDWINDOW | WS_SIZEBOX,
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

void COverlappedWindow::OnNCCreate(HWND _handle) {
    handle = _handle;
}

void COverlappedWindow::OnCreate()
{
    timerId = SetTimer(handle, 0, 50, 0);
}

// Отрисовка эллипса в целевом контексте с центром в координате (x, y). Эллипс имеет радиус, указанный в параметре r класса.
void COverlappedWindow::drawEllipse(HDC targetDC, int x, int y)
{
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    HBRUSH innBrush = CreateSolidBrush(RGB(0, 0, 255));
    HGDIOBJ oldPen = SelectObject(targetDC, pen);
    HGDIOBJ oldBrush = SelectObject(targetDC, innBrush);
    Ellipse(targetDC, x - r, y - r, x + r, y + r);
    SelectObject(targetDC, oldPen);
    SelectObject(targetDC, oldBrush);
    DeleteObject(innBrush);
    DeleteObject(pen);
}

void COverlappedWindow::OnPaint()
{
    RECT clientRect;
    PAINTSTRUCT paintStruct;
    HDC windowDC = ::BeginPaint(handle, &paintStruct);
    ::GetClientRect(handle, &clientRect);
    HDC displayBufferDC = CreateCompatibleDC(windowDC);
    HBITMAP displayBuffer = CreateCompatibleBitmap(windowDC, clientRect.right - clientRect.left, 
        clientRect.bottom - clientRect.top);
    HGDIOBJ oldDisplayBuffer = SelectObject(displayBufferDC, displayBuffer);
    int R = min((clientRect.right - clientRect.left) / 2, (clientRect.bottom - clientRect.top) / 2) - r;
    int x = (clientRect.left + clientRect.right) / 2 + R * cos(time);
    int y = (clientRect.top + clientRect.bottom) / 2 + R * sin(time);
    FillRect(displayBufferDC, &clientRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
    drawEllipse(displayBufferDC, x, y);
    BitBlt(windowDC, clientRect.left, clientRect.top, clientRect.right - clientRect.left, 
        clientRect.bottom - clientRect.top, displayBufferDC, 0, 0, SRCCOPY);
    SelectObject(displayBufferDC, oldDisplayBuffer);
    DeleteObject(displayBuffer);
    DeleteDC(displayBufferDC);
    ::EndPaint(handle, &paintStruct);
}

void COverlappedWindow::OnTimer()
{
    RECT rect;
    ::GetClientRect(handle, &rect);
    time += deltaTime;
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
            window->OnNCCreate(handle);
            return DefWindowProc(handle, message, wParam, lParam);
        }
        case WM_CREATE:
        {
            COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnCreate();
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