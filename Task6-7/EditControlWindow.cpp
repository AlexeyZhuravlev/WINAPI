// Автор: Алексей Журавлев
// Описание: Реализация методов класса CNotepadWindow. Описание класса в файле "NotepadWindow.h"

#include "EditControlWindow.h"
#include "resource.h"

CEditControlWindow::CEditControlWindow()
{
    handle = 0;
}

CEditControlWindow::~CEditControlWindow()
{
}

void CEditControlWindow::initializeText() {
    HMODULE module = GetModuleHandle(0);
    HRSRC resourceHandle = FindResource(module, MAKEINTRESOURCE(IDR_TEXT3), L"TEXT");
    HGLOBAL handleData = LoadResource(module, resourceHandle);
    DWORD size = SizeofResource(module, resourceHandle);
    LPVOID data = LockResource(handleData);
    SetWindowText(handle, reinterpret_cast<LPCWSTR>(data));
}

bool CEditControlWindow::Create(HWND parentHandle) {
    handle = CreateWindowEx(0, L"EDIT", 0, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentHandle, 0, GetModuleHandle(0), 0);
    initializeText();
    SetFocus(handle);
    return handle != 0;
}

HWND CEditControlWindow::GetHandle() {
    return handle;
}