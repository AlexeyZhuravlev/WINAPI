// Автор: Алексей Журавлев
// Описание: Класс СNotepadWindow. Сильно упрощённый текстовый редактор.

#pragma once
#include <Windows.h>
#include "EditControlWindow.h"

class СNotepadWindow {
public:
    СNotepadWindow();
    ~СNotepadWindow();

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
    // Функция, вызываемая при изменении закрытии окна (приход сообщения WM_CLOSE)
    bool OnClose();
    // Функция, вызываемая при приходе сообщения от сына (приход сообщения WM_COMMAND)
    void OnCommand(WPARAM wParam, LPARAM lParam);
    // Функция, вызываемая при уничтожении окна (приход сообщения WM_DESTROY)
    void OnDestroy();

private:
    HWND handle; // Хэндл окна
    СEditControlWindow editControl; // Дочерний edit-control окна
    bool changed; // Флаг, показывающий, производился ли ввод в окно.

    void saveText();

    static const int maxFileNameSize = 256;

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
