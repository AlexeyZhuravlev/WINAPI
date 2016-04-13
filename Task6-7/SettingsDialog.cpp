// Автор: Алексей Журавлев
// Реализация методов класса CSettingDialog. Описание в классе SettingsDialog.h

#include "SettingsDialog.h"
#include "NotepadWindow.h"
#include <cstdio>

CSettingsDialog::CSettingsDialog() 
{
    handle = 0;
    preview = 0;
    parentHandle = 0;
    editControlHandle = 0;
    ::ZeroMemory(&oldSettings, sizeof(CSettings));
    ::ZeroMemory(&newSettings, sizeof(CSettings));
}

CSettingsDialog::~CSettingsDialog() 
{
}

bool CSettingsDialog::Create(HWND parentHandle_)
{
    parentHandle = parentHandle_;
    CNotepadWindow* parent = (CNotepadWindow*)GetWindowLongPtr(parentHandle, GWLP_USERDATA);
    editControlHandle = parent->GetEditControlHandle();
    INT_PTR success = ::DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), parentHandle, dialogProc,
        reinterpret_cast<LPARAM>(this));
    return( success != -1 );
}

HWND CSettingsDialog::GetHandle() {
    return handle;
}

void CSettingsDialog::OnInit(HWND handle_) 
{
    LOGFONT logFont;
    handle = handle_;
    preview = false;
    CNotepadWindow* parent = (CNotepadWindow*)GetWindowLongPtr(parentHandle, GWLP_USERDATA);
    SendMessage(GetDlgItem(handle, IDC_SLIDER_FONTSIZE), TBM_SETRANGE, TRUE, MAKELONG(8, 72));
    SendMessage(GetDlgItem(handle, IDC_SLIDER_BACKGROUND_OPACITY), TBM_SETRANGE, TRUE, MAKELONG(0, 255));
    actualFont = reinterpret_cast<HFONT>(SendMessage(editControlHandle, WM_GETFONT, 0, 0));
    GetObject(actualFont, sizeof(logFont), &logFont);
    newSettings.font = logFont;
    newSettings.opacity = 255;
    oldSettings = newSettings;
    SendMessage(GetDlgItem(handle, IDC_SLIDER_FONTSIZE), TBM_SETPOS, true, (LPARAM)logFont.lfHeight);
    SendMessage(GetDlgItem(handle, IDC_SLIDER_BACKGROUND_OPACITY), TBM_SETPOS, true, 255);
}

void CSettingsDialog::OnDestroy() 
{
}

void CSettingsDialog::updateSettings(const CSettings& settings) {
    DeleteObject(actualFont);
    actualFont = CreateFontIndirect(&settings.font);
    SendMessage(editControlHandle, WM_SETFONT, reinterpret_cast<WPARAM>(actualFont), TRUE);
    SetLayeredWindowAttributes(parentHandle, 0, settings.opacity, LWA_ALPHA);
}

INT_PTR CSettingsDialog::OnCommand(WPARAM wParam) 
{
    switch LOWORD(wParam) {
        case IDC_BUTTON_FONT:
        {
            CHOOSECOLORW choosecolor = {};
            choosecolor.lStructSize = sizeof(CHOOSECOLORW);
            choosecolor.hwndOwner = parentHandle;
            choosecolor.rgbResult = RGB(0, 0, 0);
            choosecolor.Flags = CC_RGBINIT;
            ::ChooseColor(&choosecolor);
            break;
        }
        case IDC_CHECK1:
        {
            preview = (IsDlgButtonChecked(handle, IDC_CHECK1) == BST_CHECKED);
            break;
        }
        case IDOK:
        {
            updateSettings(newSettings);
            EndDialog(handle, wParam);
            OnDestroy();
            return TRUE;
        }
        case IDCANCEL:
        {
            updateSettings(oldSettings);
            EndDialog(handle, wParam);
            OnDestroy();
            return TRUE;
        }
    }
    tryUpdate();
    return FALSE;
}

void CSettingsDialog::OnScroll(WPARAM wParam, LPARAM lParam) {
    HWND commandHandle = reinterpret_cast<HWND>(lParam);
    if( commandHandle == GetDlgItem(handle, IDC_SLIDER_FONTSIZE) ) {
        newSettings.font.lfHeight = SendMessage(GetDlgItem(handle, IDC_SLIDER_FONTSIZE), TBM_GETPOS, 0, 0);
    } else if( commandHandle == GetDlgItem(handle, IDC_SLIDER_BACKGROUND_OPACITY) ) {
        newSettings.opacity = (BYTE)SendMessage(GetDlgItem(handle, IDC_SLIDER_BACKGROUND_OPACITY), TBM_GETPOS, 0, 0);
    }
    tryUpdate();
}

void CSettingsDialog::tryUpdate() {
    if (preview) {
        updateSettings(newSettings);
    }
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
        case WM_HSCROLL:
        {
            dialog->OnScroll(wParam, lParam);
            return FALSE;
        }
    }
    return FALSE;
}