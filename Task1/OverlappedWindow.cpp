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
    windowClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    return (::RegisterClassEx(&windowClass) != 0);
}

bool COverlappedWindow::Create()
{
    handle = CreateWindowEx(0, L"OverlappedWindow", L"My Window", WS_EX_OVERLAPPEDWINDOW,
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
    DeleteObject(bufferEllipse);
    DeleteObject(bufferMask);
    DeleteDC(bufferDC);
    PostQuitMessage(0);
}

// Отрисовка эллипса в целевом контексте. Эллипс имеет радиус, указанный в параметре r класса.
void COverlappedWindow::drawEllipse(HDC targetDC)
{
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    HBRUSH innBrush = CreateSolidBrush(RGB(0, 0, 255));
    SelectObject(targetDC, pen);
    SelectObject(targetDC, innBrush);
    Ellipse(targetDC, 0, 0, 2 * r, 2 * r);
    DeleteObject(innBrush);
    DeleteObject(pen);
}

void COverlappedWindow::OnCreate()
{
    // Инициализация времени
    time = 0;
    // Создаём прямоугольник, соответствующий изображению эллипса радиуса r
    RECT bufferRect;
    bufferRect.left = bufferRect.top = 0; 
    bufferRect.right = bufferRect.bottom = 2 * r; 
    // Создаём в памяти 2 контекста изображения, которые будут будут поочерёдно привязываться к одному хэндлу
    HDC windowDC = GetDC(handle);
    bufferDC = CreateCompatibleDC(windowDC);
    bufferEllipse = CreateCompatibleBitmap(windowDC, 2 * r, 2 * r);
    bufferMask = CreateCompatibleBitmap(windowDC, 2 * r, 2 * r);
    // Рисуем в первом контексте фон и эллипс
    SelectObject(bufferDC, bufferEllipse);
    FillRect(bufferDC, &bufferRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
    drawEllipse(bufferDC);
    // Второй контекст оставляем с чистым фоном
    SelectObject(bufferDC, bufferMask);
    FillRect(bufferDC, &bufferRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
    ReleaseDC(handle, windowDC);
}

// Копирование контекста буффера на окно в координаты, диктуемые текущим значением параметра time.
void COverlappedWindow::displayBufferOnWindow(HDC windowDC, RECT clientRect)
{   
    int R = min((clientRect.right - clientRect.left) / 2, (clientRect.bottom - clientRect.top) / 2) - r;
    int x = (clientRect.left + clientRect.right) / 2 + R * cos(time) - r;
    int y = (clientRect.top + clientRect.bottom) / 2 + R * sin(time) - r;
    BitBlt(windowDC, x, y, 2 * r, 2 * r, bufferDC, 0, 0, SRCCOPY);
}

void COverlappedWindow::OnPaint()
{
    RECT clientRect;
    PAINTSTRUCT paintStruct;
    HDC windowDC = ::BeginPaint(handle, &paintStruct);
    ::GetClientRect(handle, &clientRect);
    // отрисовываем пустой прямоугольник на старой позиции эллипса
    SelectObject(bufferDC, bufferMask);
    displayBufferOnWindow(windowDC, clientRect);
    // Увеличиваем время
    time += deltaTime;
    // отрисовываем эллипс в новой точке
    SelectObject(bufferDC, bufferEllipse);
    displayBufferOnWindow(windowDC, clientRect);
    ::EndPaint(handle, &paintStruct);
}

void COverlappedWindow::OnTimer()
{
    RECT rect;
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
            window->timerId = SetTimer(handle, 0, 50, 0);
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