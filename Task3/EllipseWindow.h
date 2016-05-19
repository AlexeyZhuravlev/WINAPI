// Автор: Алексей Журавлев
// Описание: Класс СEllipseWindow. Используется для регистрации Win32 API окна с анимацией эллипса, 
//			 создания экземляров окна, его отображения и корректной работы.

#pragma once
#include <Windows.h>

class CEllipseWindow {
public:
	CEllipseWindow();
	~CEllipseWindow();

	// Зарегистрировать класс окна
	static bool RegisterClass();

	// Создать экземпляр окна внутри родительского с хэндлом hWndParent в данной точке данных размеров
    bool Create(HWND hWndParent);
	// Показать окно
	void Show(int cmdShow);
    // Получить хэндл окна
    HWND getHandle();

protected:
    // Функция, вызываемая при создании контекста окна (приход сообщени WM_NCCREATE)
    void OnNCCreate(HWND handle);
    // Функция, вызываемая при создании окна (приход сообщени WM_CREATE)
    void OnCreate();
    // Фукнция, вызываемая при приходе в окно сообщения WM_PAINT
    void OnPaint();
    // Функция, вызываемая при приходе сообщения WM_TIMER
    void OnTimer();
    // Функция, вызываемая при уничтожении окна (приход сообщения WM_DESTROY)
    void OnDestroy();
    // Функция, вызываемая при нажатии на окно левой кнопкой мыши (приход сообщения WM_LBUTTONDOWN)
    void OnLButtonDown();

private:
	HWND handle; // Хэндл окна
    UINT_PTR timerId; // Идентификатор таймера сообщения об обновлении анимации
    double time; // Параметр времени. Характеризует положение эллипса
    static const double deltaTime; // Изменение времени при каждом приходе WM_TIMER
    static const int r; // Радиус эллипса

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
    void drawEllipse(HDC targetDC, int x, int y);
};
