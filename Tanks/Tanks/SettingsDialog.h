#pragma once

#include <Windows.h>
#include <Commctrl.h>
#include "resource.h"

class CGameWindow;

class CSettingsDialog {
public:
	CSettingsDialog();
	~CSettingsDialog();
	bool Create(HWND parentHandle);

protected:

	void OnInit(HWND handle);

	INT_PTR OnCommand(WPARAM wParam);

private:
	HWND handle, parentHandle;

	CGameWindow* gameWindow;

	static INT_PTR CALLBACK dialogProc(HWND handleDlg, UINT message, WPARAM wParam, LPARAM lParam);
};