// Автор: Алексей Журавлев
// Описание: Реализация методов класса CEllipseWindow. Описание класса в файле "EllipseWindow.h"

#include "EllipseWindow.h"
#include <Windows.h>
#include <cmath>

const double CEllipseWindow::deltaTime = 0.05;
const int CEllipseWindow::r = 30;

CEllipseWindow::CEllipseWindow() 
{
    time = 0;
    handle = 0;
    timerId = 0;
}

CEllipseWindow::~CEllipseWindow()
{
}

bool CEllipseWindow::RegisterClass()
{
    WNDCLASSEX windowClass;
    ::ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc = CEllipseWindow::windowProc;
    windowClass.hInstance = GetModuleHandle(0);
    windowClass.lpszClassName = L"EllipseWindow";
    windowClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    return (::RegisterClassEx(&windowClass) != 0);
}

bool CEllipseWindow::Create(HWND hWndParent)
{
    CreateWindowEx(0, L"EllipseWindow", L"Child Window", WS_CHILD | WS_BORDER,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndParent, 0, GetModuleHandle(0), this);
    return (handle != 0);
}

void CEllipseWindow::Show(int cmdShow)
{
    ShowWindow(handle, cmdShow);
}

HWND CEllipseWindow::getHandle()
{
    return handle;
}

void CEllipseWindow::OnNCCreate(HWND _handle) 
{
    handle = _handle;
}

void CEllipseWindow::OnCreate()
{
    timerId = SetTimer(handle, 0, 50, 0);
}

// Отрисовка эллипса в целевом контексте с центром в координате (x, y). Эллипс имеет радиус, указанный в параметре r класса.
void CEllipseWindow::drawEllipse(HDC targetDC, int x, int y)
{
    HGDIOBJ oldPen, oldBrush;
    HBRUSH innBrush;
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    if( ::GetFocus() == handle ) {
        innBrush = CreateSolidBrush(RGB(255, 0, 0));
    }
    else {
        innBrush = CreateSolidBrush(RGB(0, 0, 255));
    }
    oldPen = SelectObject(targetDC, pen);
    oldBrush = SelectObject(targetDC, innBrush);
    Ellipse(targetDC, x - r, y - r, x + r, y + r);
    SelectObject(targetDC, oldPen);
    SelectObject(targetDC, oldBrush);
    DeleteObject(innBrush);
    DeleteObject(pen);
}

void CEllipseWindow::OnPaint()
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
    int x = int((clientRect.left + clientRect.right) / 2 + R * cos(time));
    int y = int((clientRect.top + clientRect.bottom) / 2 + R * sin(time));
    FillRect(displayBufferDC, &clientRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
    drawEllipse(displayBufferDC, x, y);
    BitBlt(windowDC, clientRect.left, clientRect.top, clientRect.right - clientRect.left, 
        clientRect.bottom - clientRect.top, displayBufferDC, 0, 0, SRCCOPY);
    SelectObject(displayBufferDC, oldDisplayBuffer);
    DeleteObject(displayBuffer);
    DeleteDC(displayBufferDC);
    ::EndPaint(handle, &paintStruct);
}

void CEllipseWindow::OnTimer()
{
    RECT rect;
    ::GetClientRect(handle, &rect);
    time += deltaTime;
    InvalidateRect(handle, &rect, FALSE);
}

void CEllipseWindow::OnDestroy()
{
    KillTimer(handle, timerId);
    PostQuitMessage(0);
}

void CEllipseWindow::OnLButtonDown()
{
    SetFocus(handle);
}

// Оконная процедура. Обработка сообщений, приходящих в окно.
LRESULT CEllipseWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_NCCREATE:
        {
            CEllipseWindow* window = (CEllipseWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
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
            CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnCreate();
            return DefWindowProc(handle, message, wParam, lParam);
        }
        case WM_DESTROY:
        {
            CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnDestroy();
            return 0;
        }
        case WM_PAINT:
        {
            CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnPaint();
            return 0;
        }
        case WM_TIMER:
        {
            CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnTimer();
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnLButtonDown();
            return 0;
        }
        default:
            return DefWindowProc(handle, message, wParam, lParam);
    }
}