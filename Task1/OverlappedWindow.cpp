// �����: ������� ��������return
// ��������: ���������� ������� ������ COverlappedWindow. �������� ������ � ����� "OverlappedWindow.h"

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
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0);
    // ���������� ��������� �� ������ ������� ���� ��� ������������� � ����������� ������� ���������
    SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)this);
    return (handle != 0);
}

void COverlappedWindow::Show(int cmdShow)
{
    ShowWindow(handle, cmdShow);
}

void COverlappedWindow::OnDestroy()
{
}

// ������� ���������. ��������� ���������, ���������� � ����.
LRESULT COverlappedWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_DESTROY:
        {
            COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
            window->OnDestroy();
            PostQuitMessage(0);
            return 0;
        }
    default:
        return DefWindowProc(handle, message, wParam, lParam);
    }
}