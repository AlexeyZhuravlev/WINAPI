// Автор: Алексей Журавлев
// Описание: Главный исполняемый файл с точкой входа. Демонстрация работы с классом NotepadWindow

#include <Windows.h>
#include "NotepadWindow.h"
#include "resource.h"


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR commandLine, int cmdShow)
{
    if (!CNotepadWindow::RegisterClass()) {
        return -1;
    }
    CNotepadWindow window;
    if (!window.Create()) {
        return -1;
    }
    window.Show(cmdShow);
    HACCEL accelHandle = LoadAccelerators(instance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
    MSG message;
    BOOL getMessageResult = 0;
    while ((getMessageResult = ::GetMessage(&message, 0, 0, 0)) != 0) {
        if (getMessageResult == -1) {
            return -1;
        }
        if( !TranslateAccelerator(window.GetHandle(), accelHandle, &message) && 
            !::IsDialogMessage( window.GetOptionsDialogHandle(), &message ) ) {
            ::TranslateMessage(&message);
            ::DispatchMessage(&message);
        }
    }
    DestroyAcceleratorTable(accelHandle);
    return 0;
}