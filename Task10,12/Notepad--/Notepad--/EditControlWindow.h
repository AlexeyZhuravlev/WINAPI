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
    // Получить хэндл окна
    HWND GetHandle();

private:
    void initializeText(); // Проинициализировать текст из ресурса
    HWND handle; // Хэндл окна
};