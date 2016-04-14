// �����: ������� ��������
// ��������: ������ CNotepadWindow. C����� ���������� ��������� ��������.

#pragma once

#include <Windows.h>
#include "EditControlWindow.h"
#include "SettingsDialog.h"
#include "resource.h"

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
    // 
    HWND GetOptionsDialogHandle();

    HWND GetEditControlHandle();

    COLORREF GetBgColor();

    COLORREF GetTextColor();

    BYTE GetOpacity();

    void SetColors(COLORREF bgColor, COLORREF textColor);

    void SetOpacity(BYTE opacity);

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

    LRESULT OnControlColorEdit(HDC hdc);
    // �������, ���������� ��� ����������� ���� (������ ��������� WM_DESTROY)
    void OnDestroy();

private:
    HWND handle; // ����� ����
    CEditControlWindow editControl; // �������� edit-control ����
    CSettingsDialog settingsDialog; // �������� ���������� ���� ��� ��������
    bool changed; // ����, ������������, ����������C� �� ���� � ����.
    HMENU menu; // ���� ����
    COLORREF textColor, bgColor;
    BYTE opacity;
    HBRUSH activeBrush;

    static const HICON smallIcon, largeIcon; // ������� ������
    static const int maxFileNameSize = 256;

    void saveText(); // ���������� ������

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
