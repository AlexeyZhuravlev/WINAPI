// �����: ������� ��������
// ��������: ����� �EditControlWindow. �������� ��� �������� Edit-control ������� ����.

#pragma once
#include <Windows.h>

class �EditControlWindow {
public:
    �EditControlWindow();
    ~�EditControlWindow();

    // ������� ��������� ����
    bool Create(HWND parentHandle);
    // �������� ����
    void Show(int cmdShow);
    // �������� ����� ����
    HWND GetHandle();

private:
    HWND handle; // ����� ����
};