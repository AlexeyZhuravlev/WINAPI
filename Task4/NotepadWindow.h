// Автор: Алексей Журавлев
// Описание: Класс CNotepadWindow. Cильно упрощённый текстовый редактор.

#pragma once
#include <Windows.h>
#include "EditControlWindow.h"

class CNotepadWindow {
public:
    CNotepadWindow();
    ~CNotepadWindow();

    // Зарегистрировать класс окна
    static bool RegisterClass();

    // Cоздать экземпляр окна
    bool Create();
    // Показать окно
    void Show(int cmdShow);

protected:
    // Функция, вызываемая при Cоздании контекCта окна (приход Cообщения WM_NCCREATE)
    void OnNCCreate(HWND handle);
    // Функция, вызываемая при Cоздании окна (приход Cообщени WM_CREATE)
    void OnCreate();
    // Функция, вызываемая при изменении размера окна (приход Cообщения WM_SIZE)
    void OnSize();
    // Функция, вызываемая при изменении закрытии окна (приход Cообщения WM_CLOSE)
    bool OnClose();
    // Функция, вызываемая при приходе Cообщения от Cына (приход Cообщения WM_COMMAND)
    void OnCommand(WPARAM wParam, LPARAM lParam);
    // Функция, вызываемая при уничтожении окна (приход Cообщения WM_DESTROY)
    void OnDestroy();

private:
    HWND handle; // Хэндл окна
    CEditControlWindow editControl; // Дочерний edit-control окна
    bool changed; // Флаг, показывающий, производилCя ли ввод в окно.

    void saveText();

    static const int maxFileNameSize = 256;

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
