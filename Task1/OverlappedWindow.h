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
	// Функция, вызываемая при уничтожении окна
	void OnDestroy();

private:
	HWND handle; // Хэндл окна
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
