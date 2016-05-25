#include "SettingsDialog.h"
#include "GameWindow.h"
#include "MainWindow.h"

CSettingsDialog::CSettingsDialog()
{
	handle = 0;
}

CSettingsDialog::~CSettingsDialog()
{
	handle = 0;
}

bool CSettingsDialog::Create(HWND parentHandle_)
{
	parentHandle = parentHandle_;
	CMainWindow* parent = (CMainWindow*)GetWindowLongPtr(parentHandle, GWLP_USERDATA);
	gameWindow = parent->gameWindow;
	CreateDialogParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), parentHandle, dialogProc,
		reinterpret_cast<LPARAM>(this));
	ShowWindow(handle, SW_SHOW);
	return(handle != 0);
}

void CSettingsDialog::OnInit(HWND handle_)
{
	handle = handle_;
	SendMessage(GetDlgItem(handle, IDC_SLIDER_GRAVITY), TBM_SETRANGE, true, MAKELONG(50, 200));
	SendMessage(GetDlgItem(handle, IDC_SLIDER_GRAVITY), TBM_SETPOS, true, LONG(gameWindow->gravitation));
	SendMessage(GetDlgItem(handle, IDC_SLIDER_STRENGTH), TBM_SETRANGE, true, MAKELONG(10, 30));
	SendMessage(GetDlgItem(handle, IDC_SLIDER_STRENGTH), TBM_SETPOS, true, LONG(gameWindow->maxVelocity));
	SendMessage(GetDlgItem(handle, IDC_SLIDER_RADIUS), TBM_SETRANGE, true, MAKELONG(5, 30));
	SendMessage(GetDlgItem(handle, IDC_SLIDER_RADIUS), TBM_SETPOS, true, LONG(gameWindow->maxRadius * 100));
	SendMessage(GetDlgItem(handle, IDC_SLIDER_DAMAGE), TBM_SETRANGE, true, MAKELONG(1, 10));
	SendMessage(GetDlgItem(handle, IDC_SLIDER_DAMAGE), TBM_SETPOS, true, LONG(gameWindow->maxDamage * 10));
}

INT_PTR CSettingsDialog::OnCommand(WPARAM wParam)
{
	switch LOWORD(wParam) {
		case IDOK:
		{
			gameWindow->gravitation = SendMessage(GetDlgItem(handle, IDC_SLIDER_GRAVITY), TBM_GETPOS, 0, 0);
			gameWindow->maxVelocity = SendMessage(GetDlgItem(handle, IDC_SLIDER_STRENGTH), TBM_GETPOS, 0, 0);
			gameWindow->maxRadius = double(SendMessage(GetDlgItem(handle, IDC_SLIDER_RADIUS), TBM_GETPOS, 0, 0)) / 100;
			gameWindow->maxDamage = double(SendMessage(GetDlgItem(handle, IDC_SLIDER_DAMAGE), TBM_GETPOS, 0, 0)) / 10;
			EndDialog(handle, wParam);
			return TRUE;
		}
		case IDCANCEL:
		{
			EndDialog(handle, wParam);
			return TRUE;
		}
	}
	return FALSE;
}

INT_PTR CSettingsDialog::dialogProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG) {
		CSettingsDialog* dialog = reinterpret_cast<CSettingsDialog*>(lParam);
		SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)dialog);
		dialog->OnInit(handle);
		return TRUE;
	}
	CSettingsDialog* dialog = reinterpret_cast<CSettingsDialog*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	switch (message) {
		case WM_COMMAND:
		{
			return dialog->OnCommand(wParam);
		}
	}
	return FALSE;
}