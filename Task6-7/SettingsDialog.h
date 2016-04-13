// Автор: Алексей Журавлев
// Класс SettingsDialog. Используется для отображения и обработки сообщений диалога настроек родительского окна

#pragma once

#include <Windows.h>
#include <Commctrl.h>
#include "resource.h"

struct CSettings {
    LOGFONT font;
    BYTE opacity;
};

class CSettingsDialog {
public:
    CSettingsDialog();
    ~CSettingsDialog();
    bool Create(HWND parentHandle);
    HWND GetHandle();

protected:
    void OnInit(HWND handle_);
    INT_PTR OnCommand(WPARAM wParam);
    void OnDestroy();
    void OnScroll(WPARAM wParam, LPARAM lParam);

private:
    HWND handle, parentHandle, editControlHandle;
    bool preview;
    CSettings oldSettings, newSettings;
    HFONT actualFont;

    void updateSettings(const CSettings& settings);
    void tryUpdate();

    static INT_PTR CALLBACK dialogProc(HWND handleDlg, UINT message, WPARAM wParam, LPARAM lParam);
};