// Автор: Алексей Журавлев
// Описание: 

#pragma once

#include <Windows.h>
#include <array>
#include <ctime>
#include <cstdlib>
#include "MainWindow.h"
#include "SettingsDialog.h"

class CGameWindow {
public:
	CGameWindow();
	~CGameWindow();

	static bool RegisterClass();

	bool Create(HWND handleParent);

	void Show(int cmdShow);

	void NewGame();

	HWND GetHandle();

	friend class CSettingsDialog;

	friend LRESULT CMainWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

protected:

	void OnNCCreate(HWND handle);

	void OnCreate();
	
	void OnPaint();

	void OnDestroy();

	void OnSize();

	void OnTimer();

	void OnKeydown(WPARAM wParam, LPARAM lParam);

	void OnKeyup(WPARAM wParam, LPARAM lParam);

private:
	HWND handle;
	static const int landscapePointsNumber = 200;
	static const double deltaAngle, deltaVelocity;
	std::array<float, landscapePointsNumber> landscapeShapeOrdinates;
	unsigned int activeSegmentNumber;
	double maxRadius, maxVelocity, gravitation, maxDamage;

	std::array<double, 2> playerHealth;
	std::array<double, 2> playerAngle;
	std::array<COLORREF, 2> playerColor;
	std::array<std::pair<double, double>, 2> playerBarrel;
	std::array<std::pair<double, double>, 2> playerCenter;
	std::array<std::pair<double, double>, landscapePointsNumber> landscape;
	int curentTurn;

	POINT* drawLandscape(HDC windowDC, COLORREF color);
	void drawTank(HDC windowDC, COLORREF color, POINT position, double angle, int player);
	void drawArrow(HDC windowDC, POINT position, COLORREF color);
	void drawVelocityBar(HDC windowDC, POINT position, COLORREF color);
	void drawHPBar(HDC windowDC, POINT position, COLORREF color, int player);
	void drawPartialBar(HDC windowDC, HBRUSH brush, POINT position, int barWidth, int barHeight, double percent);

	double eucledianDistance(double x1, double y1, double x2, double y2);
	bool calcCoorsCheckBoom();

	UINT_PTR timerId;
	double velocityValue;
	double flightTime;
	double bombRadius;

	double ballX, ballY, ballVX, ballVY;

	enum TTimerState { TS_Inactive, TS_Velocity, TS_Flight, TS_Bomb, TS_GameOver };

	TTimerState timerState;

	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};