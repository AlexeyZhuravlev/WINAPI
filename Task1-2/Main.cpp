// Автор: Алексей Журавлев
// Описание: Главный исполняемый файл с точкой входа. Демонстрация работы с классом COverlappedWindow

#include <Windows.h>
#include "OverlappedWindow.h"


int WINAPI wWinMain( HINSTANCE instance, HINSTANCE prevInstance, LPWSTR commandLine, int cmdShow ) 
{
	if( !COverlappedWindow::RegisterClass() ) {
		return -1;
	}
	COverlappedWindow window;
	if( !window.Create() ) {
		return -1;
	}
	window.Show(cmdShow);
	MSG message;
	BOOL getMessageResult = 0;
	while( (getMessageResult = ::GetMessage( &message, 0, 0, 0 )) != 0 ) {
		if( getMessageResult == -1 ) {
			return -1;
		}
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
	return 0;
}