// Автор: Алексей Журавлев
// Описание: Класс СOverlappedWindow. Используется для регистрации Win32 API окна, 
//			 создания экземляров окна, его отображения и корректной работы.

#pragma once
#include <Windows.h>
#include <string>

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
	// Функция, вызываемая при уничтожении окна (приход сообщения WM_DESTROY)
	void OnDestroy();
    // Функция, вызываемая при создании контекста окна (приход сообщени WM_NCCREATE)
    void OnNCCreate(HWND handle);
    // Функция, вызываемая при создании окна (приход сообщени WM_CREATE)
    void OnCreate();
    // Фукнция, вызываемая при приходе в окно сообщения WM_PAINT
    void OnPaint();
    // Функция, вызываемая при приходе сообщения WM_TIMER
    void OnTimer();

private:
	HWND handle; // Хэндл окна
    UINT_PTR timerId; // Идентификатор таймера сообщения об обновлении анимации
    double time; // Параметр времени. Характеризует положение эллипса
    static const double deltaTime; // Изменение времени при каждом приходе WM_TIMER
    static const int r = 30; // Радиус эллипса

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
    void drawEllipse(HDC targetDC, int x, int y);
};
