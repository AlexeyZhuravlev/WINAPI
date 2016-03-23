// Àâbòîğ: Àëåêñåé Æóğàâëåâ
// Îïèñàíèå: Ğåàëèçàöèÿ ìåòîäîâ êëàññà ÑNotepadWindow. Îïèñàíèå êëàññà â ôàéëå "NotepadWindow.h"

#include "EditControlWindow.h"

ÑEditControlWindow::ÑEditControlWindow() 
{
    handle = 0;
}

ÑEditControlWindow::~ÑEditControlWindow()
{
}

bool ÑEditControlWindow::Create(HWND parentHandle) {
    handle = CreateWindowEx(0, L"EDIT", 0, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentHandle, 0, GetModuleHandle(0), 0);
    SetFocus(handle);
    return handle != 0;
}

void ÑEditControlWindow::Show(int cmdShow) {
    ShowWindow(handle, cmdShow);
}

HWND ÑEditControlWindow::GetHandle() {
    return handle;
}