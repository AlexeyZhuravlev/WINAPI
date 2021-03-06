// �����: ������� ��������
// ��������: ������ CNotepadWindow. C����� ���������� ��������� ��������.

#pragma once
#include <Windows.h>
#include "EditControlWindow.h"

class CNotepadWindow {
public:
    CNotepadWindow();
    ~CNotepadWindow();

    // ���������������� ����� ����
    static bool RegisterClass();

    // C������ ��������� ����
    bool Create();
    // �������� ����
    void Show(int cmdShow);
    // �������� ����� ����
    HWND GetHandle();

protected:
    // �������, ���������� ��� �������� ��������� ���� (������ ��������� WM_NCCREATE)
    void OnNCCreate(HWND handle);
    // �������, ���������� ��� �������� ���� (������ ��������� WM_CREATE)
    void OnCreate();
    // �������, ���������� ��� ��������� ������� ���� (������ ��������� WM_SIZE)
    void OnSize();
    // �������, ���������� ��� ��������� �������� ���� (������ ��������� WM_CLOSE)
    bool OnClose();
    // �������, ���������� ��� ������� ��������� �� ����������� (������ ��������� WM_COMMAND)
    void OnCommand(WPARAM wParam, LPARAM lParam);
    // �������, ���������� ��� ������� ��������� �� ���� (������ ��������� WM_COMMAND, lParam=0)
    void OnCommandMenu(WPARAM wParam);
    // �������, ���������� ��� ������� ��������� �� ������������ (������ ��������� WM_COMMAND, lParam=1)
    void OnCommandAccelerator(WPARAM wParam);
    // �������, ���������� ��� ����������� ���� (������ ��������� WM_DESTROY)
    void OnDestroy();

private:
    HWND handle; // ����� ����
    CEditControlWindow editControl; // �������� edit-control ����
    bool changed; // ����, ������������, ����������C� �� ���� � ����.
    HMENU menu; // ���� ����

    static const HICON smallIcon, largeIcon; // ������� ������
    static const int maxFileNameSize = 256;

    void saveText(); // ���������� ������

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
