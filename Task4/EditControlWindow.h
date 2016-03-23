// Автор: Алексей Журавлев
// Описание: Класс СEditControlWindow. Создаётся как дочерний Edit-control другого окна.

#pragma once
#include <Windows.h>

class СEditControlWindow {
public:
    СEditControlWindow();
    ~СEditControlWindow();

    // Создать экземпляр окна
    bool Create(HWND parentHandle);
    // Показать окно
    void Show(int cmdShow);
    // Получить хэндл окна
    HWND GetHandle();

private:
    HWND handle; // Хэндл окна
};