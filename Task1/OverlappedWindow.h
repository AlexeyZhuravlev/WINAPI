// �����: ������� ��������
// ��������: ����� �OverlappedWindow. ������������ ��� ����������� Win32 API ����, 
//			 �������� ���������� ����, ��� ����������� � ���������� ������.

#pragma once
#include <Windows.h>
#include <string>

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
	// �������, ���������� ��� ����������� ����
	void OnDestroy();

private:
	HWND handle; // ����� ����
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
