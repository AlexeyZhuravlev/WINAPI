// �����: ������� ��������
// ��������: ����� COverlappedWindow.

#pragma once
#include "EllipseWindow.h"
#include <Windows.h>
#include <array>

class COverlappedWindow {
public:
    COverlappedWindow();
    ~COverlappedWindow();

    // ���������������� ����� ����
    static bool RegisterClass();

    // ������� ��������� ����
    bool Create();
    // �������� ����
    void Show(int cmdShow);

protected:
    // �������, ���������� ��� �������� ��������� ���� (������ �������� WM_NCCREATE)
    void OnNCCreate(HWND handle);
    // �������, ���������� ��� �������� ���� (������ �������� WM_CREATE)
    void OnCreate();
    // �������, ���������� ��� ��������� ������� ���� (������ ��������� WM_SIZE)
    void OnSize();
    // �������, ���������� ��� ����������� ���� (������ ��������� WM_DESTROY)
    void OnDestroy();

private:
    HWND handle;
    std::array<CEllipseWindow, 4> childWindows;

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
