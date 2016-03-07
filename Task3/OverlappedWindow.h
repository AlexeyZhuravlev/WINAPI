// Автор: Алексей Журавлев
// Описание: Класс COverlappedWindow. Отображает 4 дочерних окна, соответствующих экземплярам CEllipseWindow

#pragma once
#include "EllipseWindow.h"
#include <Windows.h>
#include <array>

class COverlappedWindow {
public:
    COverlappedWindow();
    ~COverlappedWindow();

    // Зарегистрировать класс окна
    static bool RegisterClass();

    // Создать экземпляр окна
    bool Create();
    // Показать окно
    void Show(int cmdShow);

protected:
    // Функция, вызываемая при создании контекста окна (приход сообщени WM_NCCREATE)
    void OnNCCreate(HWND handle);
    // Функция, вызываемая при создании окна (приход сообщени WM_CREATE)
    void OnCreate();
    // Функция, вызываемая при изменении размера окна (приход сообщения WM_SIZE)
    void OnSize();
    // Функция, вызываемая при уничтожении окна (приход сообщения WM_DESTROY)
    void OnDestroy();

private:
    HWND handle; // Хэндл окна
    std::array<CEllipseWindow, 4> childWindows; // Объекты - дочерние окна

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
