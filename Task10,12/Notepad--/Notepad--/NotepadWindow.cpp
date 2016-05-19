// Автор: Алексей Журавлев
// Описание: Реализация методов класса CNotepadWindow. Описание класса в файле "NotepadWindow.h"

#include "NotepadWindow.h"

const HICON CNotepadWindow::largeIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
const HICON CNotepadWindow::smallIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));

CNotepadWindow::CNotepadWindow()
{
    handle = 0;
    menu = 0;
    changed = 0;
    textColor = 0;
    bgColor = 0;
    opacity = 0;
    activeBrush = 0;
    editControl = CEditControlWindow();
    settingsDialog = CSettingsDialog();
	helpersCreated = false;
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
    CreateWindowEx(WS_EX_LAYERED, L"NotepadWindow", windowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
    SetLayeredWindowAttributes(handle, 0, 255, LWA_ALPHA);
    opacity = 255;
    changed = false;
    delete[] windowTitle;
    return (handle != 0);
}

HWND CNotepadWindow::GetHandle() {
    return handle;
}

HWND CNotepadWindow::GetOptionsDialogHandle() {
    return settingsDialog.GetHandle();
}

HWND CNotepadWindow::GetEditControlHandle() {
    return editControl.GetHandle();
}

COLORREF CNotepadWindow::GetBgColor() {
    return bgColor;
}

COLORREF CNotepadWindow::GetTextColor() {
    return textColor;
}

BYTE CNotepadWindow::GetOpacity() {
    return opacity;
}

void CNotepadWindow::SetColors(COLORREF bgColor_, COLORREF textColor_) {
    bgColor = bgColor_;
    textColor = textColor_;
}

void CNotepadWindow::SetOpacity(BYTE opacity_) {
    opacity = opacity_;
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
    textColor = RGB(0, 0, 0);
    bgColor = RGB(255, 255, 255);
    editControl.Create(handle);
    menu = LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(IDR_MENU1));
    HFONT hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    SendMessage(editControl.GetHandle(), WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
	createHelpers();
}

void CNotepadWindow::OnSize()
{
    RECT rect;
    ::GetClientRect(handle, &rect);
    SetWindowPos(editControl.GetHandle(), HWND_TOP, rect.left, rect.top, rect.right - rect.left,
        rect.bottom - rect.top, 0);
}

int CNotepadWindow::getTextLength() {
	int length = SendMessage(editControl.GetHandle(), WM_GETTEXTLENGTH, 0, 0);
	return length;
}

wchar_t* CNotepadWindow::getText() {
	int length = getTextLength();
	wchar_t *text = new wchar_t[length + 1];
	SendMessage(editControl.GetHandle(), WM_GETTEXT, length + 1, reinterpret_cast<LPARAM>(text));
	return text;
}

bool CNotepadWindow::openFile(wchar_t* fileName)
{
	OPENFILENAME ofn = {};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = fileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = maxFileNameSize;
	ofn.hwndOwner = handle;
	return GetSaveFileName(&ofn);
}

// Функция для Cохранения данных editControl в текCтовый файл. Имя файла получает из диалога C пользователем.
void CNotepadWindow::saveText()
{
	int length = getTextLength();
	wchar_t *text = getText();
    wchar_t fileName[maxFileNameSize];
	if (openFile(fileName)) {
        HANDLE file = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        unsigned short a = 0xFEFF;
        WriteFile(file, &a, 2, 0, 0);
        WriteFile(file, text, length * sizeof(wchar_t), 0, 0);
        CloseHandle(file);
    }
    delete[] text;
}

void CNotepadWindow::getTextFromFile()
{
	wchar_t fileName[maxFileNameSize];
	if (openFile(fileName)) {
		HANDLE file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		DWORD actualFileSize;
		DWORD fileSize = GetFileSize(file, 0);
		// Assume reading unicode file
		wchar_t* buffer = new wchar_t[fileSize / sizeof(wchar_t) + 1];
		ReadFile(file, buffer, fileSize, &actualFileSize, 0);
		buffer[fileSize / sizeof(wchar_t)] = '\0';
		SendMessage(editControl.GetHandle(), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(buffer));
		delete[] buffer;
		CloseHandle(file);
	}
}

void CNotepadWindow::createHelpers() {
	for (int i = 0; i < helperProcessNumber; i++) {
		STARTUPINFO startUp;
		PROCESS_INFORMATION processInfo;
		startUp.cb = sizeof(startUp);
		ZeroMemory(&startUp, sizeof(startUp));
		ZeroMemory(&processInfo, sizeof(processInfo));
		HANDLE creation = CreateEvent(0, false, false, L"Global\\EventsInitialized");
		CreateProcess(L"C:\\Users\\Алексей\\Documents\\Visual Studio 2013\\Projects\\spellChecker\\Debug\\spellChecker.exe", 
			0, 0, 0, FALSE, 0, 0, 0, &startUp, &processInfo);
		WaitForSingleObject(creation, INFINITY);
		helperProcessIds[i] = processInfo.dwProcessId;
	}
}

void CNotepadWindow::filterText()
{
	wchar_t* text = getText();
	int textSize = wcslen(text);
	wchar_t* borders[helperProcessNumber + 1];
	for (int i = 0; i < helperProcessNumber; i++) {
		borders[i] = text + textSize / helperProcessNumber * i;
	}
	borders[helperProcessNumber] = text + textSize;
	for (int i = 1; i < helperProcessNumber; i++) {
		while( iswalpha(*borders[i]) || iswdigit(*borders[i]) ) {
			borders[i]++;
		}
	}
	HANDLE mappingFiles[helperProcessNumber];
	wchar_t* buffers[helperProcessNumber];
	for (int i = 0; i < helperProcessNumber; i++) {
		wchar_t fileName[40];
		swprintf_s(fileName, L"Local\\Text%d", helperProcessIds[i]);
		int partLen = borders[i + 1] - borders[i] + 1;
		HANDLE file = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, partLen * sizeof(wchar_t), fileName);
		wchar_t* buffer = static_cast<wchar_t*>(MapViewOfFile(file, FILE_MAP_ALL_ACCESS, 0, 0, 0));
		memcpy(buffer, borders[i], (partLen - 1) * sizeof(wchar_t));
		buffer[partLen - 1] = '\0';
		buffers[i] = buffer;
		mappingFiles[i] = file;
	}
	HANDLE waitEvents[helperProcessNumber];
	for (int i = 0; i < helperProcessNumber; i++) {
		wchar_t eventName[40];
		swprintf_s(eventName, L"Global\\TextProceeded%d", helperProcessIds[i]);
		waitEvents[i] = CreateEvent(0, false, false, eventName);
	}
	for (int i = 0; i < helperProcessNumber; i++) {
		wchar_t eventName[40];
		swprintf_s(eventName, L"Global\\NewText%d", helperProcessIds[i]);
		HANDLE newTextEvent = OpenEvent(EVENT_ALL_ACCESS, true, eventName);
		SetEvent(newTextEvent);
		CloseHandle(newTextEvent);
	}
	WaitForMultipleObjects(helperProcessNumber, waitEvents, true, INFINITE);
	wchar_t* current = text;
	for (int i = 0; i < helperProcessNumber; i++) {
		int blockLen = wcslen(buffers[i]);
		memcpy(current, buffers[i], blockLen * sizeof(wchar_t));
		current += blockLen;
		CloseHandle(mappingFiles[i]);
		CloseHandle(waitEvents[i]);
	}
	*current = '\0';
	SendMessage(editControl.GetHandle(), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text));
	delete[] text;
}

bool CNotepadWindow::OnClose()
{
    if (changed) {
        int msgBoxID = MessageBox(handle, L"Cохранить введённый текст?", L"Notepad--", MB_YESNOCANCEL);
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
    return true;
}

void CNotepadWindow::OnCommandMenu(WPARAM wParam)
{
    switch LOWORD(wParam) {
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
        case ID_VIEW_SETTINGS:
        {
            settingsDialog.Create(handle);
            break;
        }
		case ID_FILE_WORDSCOUNT:
		{
			wchar_t *text = getText();
			int count = WordsCount(text);
			wchar_t stringedWordsCount[10];
			swprintf_s(stringedWordsCount, L"%d", count);
			MessageBox(handle, stringedWordsCount, L"Words count", 0);
			break;
		}
		case ID_FILE_OPEN:
		{
			getTextFromFile();
			break;
		}
		case ID_FILE_CLEAR:
		{
			filterText();
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

LRESULT CNotepadWindow::OnControlColorEdit(HDC hdc) {
    SetTextColor(hdc, textColor);
    SetBkColor(hdc, bgColor);
    if (activeBrush != 0) {
        DeleteObject(activeBrush);
    }
    activeBrush = CreateSolidBrush(bgColor);
    return (LRESULT)activeBrush;
}

void CNotepadWindow::OnDestroy()
{
    DestroyIcon(smallIcon);
    DestroyIcon(largeIcon);
    DestroyMenu(menu);
    HFONT font = reinterpret_cast<HFONT>(SendMessage(editControl.GetHandle(), WM_GETFONT, 0, 0));
    DeleteObject(font);
    if (activeBrush != 0) {
        DeleteObject(activeBrush);
    }
	HANDLE termEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Global\\TerminateProcesses");
	SetEvent(termEvent);
	CloseHandle(termEvent);
    PostQuitMessage(0);
}

// Оконная процедура. Обработка сообщений, приходящих в окно.
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
        case WM_CTLCOLOREDIT:
        {
            return window->OnControlColorEdit(reinterpret_cast<HDC>(wParam));
        }
        default:
        {
            return DefWindowProc(handle, message, wParam, lParam);
        }
    }
}