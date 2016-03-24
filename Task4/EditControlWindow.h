// Автор: Алексей Журавлев
// ОпиCание: Класс CEditControlWindow. Cоздаётся как дочерний Edit-control другого окна.

#pragma once
#include <Windows.h>

class CEditControlWindow {
public:
    CEditControlWindow();
    ~CEditControlWindow();

    // Cоздать экземпляр окна
    bool Create(HWND parentHandle);
    // Показать окно
    void Show(int cmdShow);
    // Получить хэндл окна
    HWND GetHandle();

private:
    HWND handle; // Хэндл окна
};