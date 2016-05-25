// Автор: Алексей Журавлев
// Описание: 

#pragma once

#include <Windows.h>
#include "resource.h"
#include "SettingsDialog.h"

class CGameWindow;
class CSettingsDialog;

class CMainWindow {
public:
	CMainWindow();
	~CMainWindow();

	static bool RegisterClass();

	bool Create();

	void Show(int cmdShow);

	friend class CGameWindow;
	friend class CSettingsDialog;

protected:

	void OnNCCreate(HWND handle);

	void OnCreate();

	void OnSize();

	void OnDestroy();

	void OnCommand(WPARAM wParam, LPARAM lParam);

	void OnCommandMenu(WPARAM wParam);

private:
	HWND handle;

	CGameWindow* gameWindow;

	CSettingsDialog* settingsDialog;

	static const HICON smallIcon, largeIcon;

	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};