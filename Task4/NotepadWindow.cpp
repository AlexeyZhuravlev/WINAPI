// Автор: Алексей Журавлев
// Описание: Реализация методов класса СNotepadWindow. Описание класса в файле "NotepadWindow.h"

#include "NotepadWindow.h"

СNotepadWindow::СNotepadWindow()
{
    handle = 0;
    changed = false;
    editControl = СEditControlWindow();
}

СNotepadWindow::~СNotepadWindow()
{
}

bool СNotepadWindow::RegisterClass()
{
    WNDCLASSEX windowClass;
    ::ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc = СNotepadWindow::windowProc;
    windowClass.hInstance = GetModuleHandle(0);
    windowClass.lpszClassName = L"NotepadWindow";
    return (::RegisterClassEx(&windowClass) != 0);
}

bool СNotepadWindow::Create()
{
    CreateWindowEx(0, L"NotepadWindow", L"Notepad--", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
    return (handle != 0);
}

void СNotepadWindow::Show(int cmdShow)
{
    ShowWindow(handle, cmdShow);
    editControl.Show(cmdShow);
}

void СNotepadWindow::OnNCCreate(HWND _handle) 
{
    handle = _handle;
}

void СNotepadWindow::OnCreate()
{
    editControl.Create(handle);
}

void СNotepadWindow::OnSize()
{
    RECT rect;
    ::GetClientRect(handle, &rect);
    SetWindowPos(editControl.GetHandle(), HWND_TOP, rect.left, rect.top, rect.right - rect.left, 
        rect.bottom - rect.left, 0);
}

// Функция для сохранения данных editControl в текстовый файл. Имя файла получает из диалога с пользователем.
void СNotepadWindow::saveText()
{
    int length = SendMessage(editControl.GetHandle(), WM_GETTEXTLENGTH, 0, 0);
    wchar_t *text = new wchar_t[length + 1];
    SendMessage(editControl.GetHandle(), WM_GETTEXT, length + 1, reinterpret_cast<LPARAM>(text));
    wchar_t fileName[maxFileNameSize];
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(fileName);
    if( GetSaveFileName(&ofn) ) {
        HANDLE file = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        WriteFile(file, text, length * sizeof(wchar_t), 0, 0);
        CloseHandle(file);
    }
    delete[] text;
}

bool СNotepadWindow::OnClose()
{
    if( changed ) {
        int msgBoxID = MessageBox(handle, L"Сохранить введённый текст?", L"Notepad--", MB_YESNOCANCEL);
        switch (msgBoxID)
        {
            case IDYES:
            {
                saveText();
                return true;
            }
            case IDNO:
            {
                return true;
            }
            case IDCANCEL:
            {
                return false;
            }
        }
    }
}

void СNotepadWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch HIWORD(wParam)
    {
        case EN_CHANGE:
        {
            changed = true;
        }
    }
}

void СNotepadWindow::OnDestroy() 
{
    PostQuitMessage(0);
}

// Оконная процедура. Обработка сообщений, приходящих в окно.
LRESULT СNotepadWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE) {
        СNotepadWindow* window =
            reinterpret_cast<СNotepadWindow*>((reinterpret_cast<CREATESTRUCT*>(lParam))->lpCreateParams);
        SetLastError(0);
        SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)window);
        if (GetLastError() != 0) {
            return GetLastError();
        }
        window->OnNCCreate(handle);
        return DefWindowProc(handle, message, wParam, lParam);
    }
    СNotepadWindow* window = reinterpret_cast<СNotepadWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
    switch (message) {
        case WM_CREATE:
        {
            window->OnCreate();
            return DefWindowProc(handle, message, wParam, lParam);
        }
        case WM_SIZE:
        {
            window->OnSize();
            return DefWindowProc(handle, message, wParam, lParam);
        }
        case WM_CLOSE:
        {
            if( window->OnClose() ) {
                return DefWindowProc(handle, message, wParam, lParam);
            } else {
                return 0;
            }
        }
        case WM_COMMAND:
        {
            window->OnCommand(wParam, lParam);
            return DefWindowProc(handle, message, wParam, lParam);
        }
        case WM_DESTROY:
        {
            window->OnDestroy();
            return 0;
        }
        default:
        {
            return DefWindowProc(handle, message, wParam, lParam);
        }
    }
}