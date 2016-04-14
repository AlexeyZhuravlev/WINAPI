// Автор: Алексей Журавлев
// Описание: Класса CNotepadWindow. Cильно упрощённый текстовый редактор.

#pragma once

#include <Windows.h>
#include "EditControlWindow.h"
#include "SettingsDialog.h"
#include "resource.h"

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
    // Получить хэндл окна
    HWND GetHandle();
    // 
    HWND GetOptionsDialogHandle();

    HWND GetEditControlHandle();

    COLORREF GetBgColor();

    COLORREF GetTextColor();

    BYTE GetOpacity();

    void SetColors(COLORREF bgColor, COLORREF textColor);

    void SetOpacity(BYTE opacity);

protected:
    // Функция, вызываемая при создании контекста окна (приход сообщения WM_NCCREATE)
    void OnNCCreate(HWND handle);
    // Функция, вызываемая при создании окна (приход сообщения WM_CREATE)
    void OnCreate();
    // Функция, вызываемая при изменении размера окна (приход сообщения WM_SIZE)
    void OnSize();
    // Функция, вызываемая при изменении закрытии окна (приход сообщения WM_CLOSE)
    bool OnClose();
    // Функция, вызываемая при приходе сообщения от контроллера (приход сообщения WM_COMMAND)
    void OnCommand(WPARAM wParam, LPARAM lParam);
    // Функция, вызываемая при приходе сообщения от меню (приход сообщения WM_COMMAND, lParam=0)
    void OnCommandMenu(WPARAM wParam);
    // Функция, вызываемая при приходе сообщения от акселератора (приход сообщения WM_COMMAND, lParam=1)
    void OnCommandAccelerator(WPARAM wParam);

    LRESULT OnControlColorEdit(HDC hdc);
    // Функция, вызываемая при уничтожении окна (приход сообщения WM_DESTROY)
    void OnDestroy();

private:
    HWND handle; // Хэндл окна
    CEditControlWindow editControl; // Дочерний edit-control окна
    CSettingsDialog settingsDialog; // Дочернее диалоговое окно для настроек
    bool changed; // Флаг, показывающий, производилCя ли ввод в окно.
    HMENU menu; // Меню окна
    COLORREF textColor, bgColor;
    BYTE opacity;
    HBRUSH activeBrush;

    static const HICON smallIcon, largeIcon; // Объекты иконок
    static const int maxFileNameSize = 256;

    void saveText(); // Сохранение текста

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
