// �����: ������� ��������
// ���C����: ����� CEditControlWindow. C������� ��� �������� Edit-control ������� ����.

#pragma once
#include <Windows.h>

class CEditControlWindow {
public:
    CEditControlWindow();
    ~CEditControlWindow();

    // C������ ��������� ����
    bool Create(HWND parentHandle);
    // �������� ����� ����
    HWND GetHandle();

private:
    void initializeText(); // ������������������� ����� �� �������
    HWND handle; // ����� ����
};