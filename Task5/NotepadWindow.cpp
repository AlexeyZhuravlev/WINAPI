// �����: ������� ��������
// ��������: ���������� ������� ������ CNotepadWindow. �������� ������ � ����� "NotepadWindow.h"

#include "NotepadWindow.h"
#include "resource.h"

const HICON CNotepadWindow::largeIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
const HICON CNotepadWindow::smallIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));

CNotepadWindow::CNotepadWindow()
{
    handle = 0;
    menu = 0;
    changed = false;
    editControl = CEditControlWindow();
}

CNotepadWindow::~CNotepadWindow()
{
}

bool CNotepadWindow::RegisterClass()
{
    WNDCLASSEX windowClass;
    ::ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc = CNotepadWindow::windowProc;
    windowClass.hInstance = GetModuleHandle(0);
    windowClass.lpszClassName = L"NotepadWindow";
    windowClass.hIcon = largeIcon;
    windowClass.hIconSm = smallIcon;
    windowClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    return (::RegisterClassEx(&windowClass) != 0);
}

bool CNotepadWindow::Create()
{
    LPTSTR windowTitle = new wchar_t[256];
    LoadString(GetModuleHandle(0), IDS_NOTEPAD_TITLE, windowTitle, 256);
    CreateWindowEx(0, L"NotepadWindow", windowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
    delete[] windowTitle;
    return (handle != 0);
}

HWND CNotepadWindow::GetHandle() {
    return handle;
}

void CNotepadWindow::Show(int cmdShow)
{
    ShowWindow(handle, cmdShow);
}

void CNotepadWindow::OnNCCreate(HWND _handle)
{
    handle = _handle;
}

void CNotepadWindow::OnCreate()
{
    editControl.Create(handle);
    menu = LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(IDR_MENU1));
}

void CNotepadWindow::OnSize()
{
    RECT rect;
    ::GetClientRect(handle, &rect);
    SetWindowPos(editControl.GetHandle(), HWND_TOP, rect.left, rect.top, rect.right - rect.left,
        rect.bottom - rect.top, 0);
}

// ������� ��� C��������� ������ editControl � ���C����� ����. ��� ����� �������� �� ������� C �������������.
void CNotepadWindow::saveText()
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
    ofn.hwndOwner = handle;
    if (GetSaveFileName(&ofn)) {
        HANDLE file = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        unsigned short a = 0xFEFF;
        WriteFile(file, &a, 2, 0, 0);
        WriteFile(file, text, length * sizeof(wchar_t), 0, 0);
        CloseHandle(file);
    }
    delete[] text;
}

bool CNotepadWindow::OnClose()
{
    if (changed) {
        int msgBoxID = MessageBox(handle, L"C�������� �������� �����?", L"Notepad--", MB_YESNOCANCEL);
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

void CNotepadWindow::OnCommandMenu(WPARAM wParam)
{
    switch LOWORD(wParam)
        {
        case ID_FILE_SAVE:
        {
            saveText();
            break;
        }
        case ID_FILE_EXIT:
        {
            SendMessage(handle, WM_CLOSE, 0, 0);
            break;
        }
    }
}

void CNotepadWindow::OnCommandAccelerator(WPARAM wParam)
{
    switch LOWORD(wParam)
    {
        case ID_ACCELERATOR_QUIT:
        {
            SendMessage(handle, WM_DESTROY, 0, 0);
            break;
        }
    }
}

void CNotepadWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch HIWORD(wParam)
    {
        case EN_CHANGE:
        {
            changed = true;
            break;
        }
        case 0:
        {
            OnCommandMenu(wParam);
            break;
        }
        case 1:
        {
            OnCommandAccelerator(wParam);
            break;
        }
    }
}

void CNotepadWindow::OnDestroy()
{
    DestroyIcon(smallIcon);
    DestroyIcon(largeIcon);
    DestroyMenu(menu);
    PostQuitMessage(0);
}

// ������� ���������. ��������� ���������, ���������� � ����.
LRESULT CNotepadWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE) {
        CNotepadWindow* window =
            reinterpret_cast<CNotepadWindow*>((reinterpret_cast<CREATESTRUCT*>(lParam))->lpCreateParams);
        SetLastError(0);
        SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)window);
        if (GetLastError() != 0) {
            return GetLastError();
        }
        window->OnNCCreate(handle);
        return DefWindowProc(handle, message, wParam, lParam);
    }
    CNotepadWindow* window = reinterpret_cast<CNotepadWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
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
            }
            else {
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