// Автор: Алексей Журавлев
// Описание: 

#include "MainWindow.h"
#include "GameWindow.h"

const HICON CMainWindow::largeIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), 
	IMAGE_ICON, 32, 32, 0));
const HICON CMainWindow::smallIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1),
	IMAGE_ICON, 16, 16, 0));


CMainWindow::CMainWindow()
{
	handle = 0;
	gameWindow = 0;
	settingsDialog = 0;
}

CMainWindow::~CMainWindow()
{
}

bool CMainWindow::RegisterClass() 
{
	WNDCLASSEX windowClass;
	::ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = CMainWindow::windowProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.hIcon = largeIcon;
	windowClass.hIconSm = smallIcon;
	windowClass.lpszClassName = L"MainWindow";
	windowClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	return (::RegisterClassEx(&windowClass) != 0 && CGameWindow::RegisterClass());
}

bool CMainWindow::Create()
{
	CreateWindowEx(0, L"MainWindow", L"Tanks", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
	gameWindow = new CGameWindow();
	settingsDialog = new CSettingsDialog();
	return (handle != 0 && gameWindow->Create(handle));
}

void CMainWindow::Show(int cmdShow) 
{
	ShowWindow(handle, cmdShow);
	gameWindow->Show(cmdShow);
}

void CMainWindow::OnSize()
{
	RECT rect;
	::GetClientRect(handle, &rect);
	SetWindowPos(gameWindow->GetHandle(), HWND_TOP, rect.left, rect.top, rect.right - rect.left, 
		rect.bottom - rect.top, 0);
}

void CMainWindow::OnDestroy()
{
	delete settingsDialog;
	delete gameWindow;
	PostQuitMessage(0);
}

void CMainWindow::OnCommandMenu(WPARAM wParam)
{
	switch LOWORD(wParam)
	{
		case ID_FILE_NEWGAME:
		{
			gameWindow->NewGame();
			break;
		}
		case ID_FILE_EXIT:
		{
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}
		case ID_EDIT_GAMESETTINGS:
		{
			settingsDialog->Create(handle);
			break;
		}
	}
}

void CMainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch HIWORD(wParam)
	{
		case 0:
		{
			OnCommandMenu(wParam);
			break;
		}
	}
}

void CMainWindow::OnNCCreate(HWND _handle)
{
	handle = _handle;
}

void CMainWindow::OnCreate()
{
}

LRESULT CMainWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE) {
		CMainWindow* window =
			reinterpret_cast<CMainWindow*>((reinterpret_cast<CREATESTRUCT*>(lParam))->lpCreateParams);
		SetLastError(0);
		SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)window);
		if (GetLastError() != 0) {
			return GetLastError();
		}
		window->OnNCCreate(handle);
		return DefWindowProc(handle, message, wParam, lParam);
	}
	CMainWindow* window = reinterpret_cast<CMainWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	switch (message) {
		case WM_CREATE:
		{
			window->OnCreate();
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_DESTROY:
		{
			window->OnDestroy();
			return 0;
		}
		case WM_KEYDOWN:
		{
			window->gameWindow->OnKeydown(wParam, lParam);
			return 0;
		}
		case WM_KEYUP:
		{
			window->gameWindow->OnKeyup(wParam, lParam);
			return 0;
		}
		case WM_COMMAND:
		{
			window->OnCommand(wParam, lParam);
		}
		case WM_SIZE:
		{
			window->OnSize();
			return 0;
		}
		default:
		{
			return DefWindowProc(handle, message, wParam, lParam);
		}
	}
}