// Автор: Алексей Журавлев
// Описание: 

#include "GameWindow.h"
#include "SettingsDialog.h"

const double CGameWindow::deltaAngle = 0.02;
const double CGameWindow::deltaVelocity = 0.2;

CGameWindow::CGameWindow() 
{
	handle = 0;
	curentTurn = 0;
	velocityValue = 0;
	timerId = 0;
	timerState = TS_Inactive;
	ballX = ballY = ballVX = ballVY = 0;
	activeSegmentNumber = 0;
	bombRadius = 0;
	maxVelocity = 20;
	gravitation = 100;
	maxRadius = 0.2;
	maxDamage = 0.5;
}

CGameWindow::~CGameWindow() 
{
}

bool CGameWindow::RegisterClass()
{
	WNDCLASSEX windowClass;
	::ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = CGameWindow::windowProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.lpszClassName = L"GameWindow";
	windowClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	return (::RegisterClassEx(&windowClass) != 0);
}

bool CGameWindow::Create(HWND parentHandle) 
{
	CreateWindowEx(0, L"GameWindow", L"Game Window", WS_CHILD,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentHandle, 0, GetModuleHandle(0), this);
	return (handle != 0);
}

void CGameWindow::NewGame()
{
	curentTurn = 0;
	playerAngle[0] = playerAngle[1] = 3.1415 / 2;
	playerHealth[0] = playerHealth[1] = 1;
	playerColor[0] = RGB(255, 0, 0);
	playerColor[1] = RGB(0, 0, 255);
	std::srand(static_cast<unsigned int>(std::time(0)));
	for (unsigned int i = 0; i < landscapePointsNumber; i++) {
		landscapeShapeOrdinates[i] = static_cast<float>(std::rand() % 1300) / 1000;
	}
	timerState = TS_Inactive;
}

void CGameWindow::Show(int cmdShow) 
{
	ShowWindow(handle, cmdShow);
}

HWND CGameWindow::GetHandle()
{
	return handle;
}

void CGameWindow::OnNCCreate(HWND _handle) 
{
	handle = _handle;
}

void CGameWindow::OnCreate() 
{
	timerId = SetTimer(handle, 0, 20, 0);
	NewGame();
}

void CGameWindow::drawTank(HDC windowDC, COLORREF color, POINT position, double angle, int player) {
	HPEN pen = CreatePen(PS_SOLID, 5, color);
	HBRUSH brush = CreateSolidBrush(color);
	HGDIOBJ oldPen, oldBrush;
	oldPen = SelectObject(windowDC, pen);
	oldBrush = SelectObject(windowDC, brush);
	RECT rect;
	::GetClientRect(handle, &rect);
	int height = rect.bottom / 50;
	int width = rect.right / 50;
	RoundRect(windowDC, position.x - width / 2, position.y - height, position.x + width / 2,
		position.y, width / 2, height / 2);
	POINT points[2];
	int length = width * 3 / 2;
	points[0] = position;
	points[0].y -= height / 2;
	points[1].x = static_cast<LONG>(points[0].x + length * std::cos(angle));
	points[1].y = static_cast<LONG>(points[0].y - length * std::sin(angle));
	playerBarrel[player].first = static_cast<double>(points[1].x) / rect.right;
	playerBarrel[player].second = static_cast<double>(points[1].y) / rect.bottom;
	playerCenter[player].first = static_cast<double>(points[0].x) / rect.right;
	playerCenter[player].second = static_cast<double>(points[0].y) / rect.bottom;
	Polyline(windowDC, points, 2);
	SelectObject(windowDC, oldBrush);
	SelectObject(windowDC, oldPen);
	DeleteObject(brush);
	DeleteObject(pen);
}

void CGameWindow::drawArrow(HDC windowDC, POINT position, COLORREF color) {
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HBRUSH brush = CreateSolidBrush(color);
	HGDIOBJ oldPen, oldBrush;
	oldPen = SelectObject(windowDC, pen);
	oldBrush = SelectObject(windowDC, brush);
	RECT rect;
	::GetClientRect(handle, &rect);
	POINT points[3];
	points[0] = position;
	points[0].y -= rect.bottom / 10;
	points[1] = points[0];
	points[1].x -= 10;
	points[1].y -= 10;
	points[2] = points[1];
	points[2].x += 20;
	Polygon(windowDC, points, 3);
	SelectObject(windowDC, oldBrush);
	SelectObject(windowDC, oldPen);
	DeleteObject(brush);
	DeleteObject(pen);
}

POINT* CGameWindow::drawLandscape(HDC windowDC, COLORREF color)
{
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HBRUSH brush = CreateSolidBrush(color);
	HGDIOBJ oldPen, oldBrush;
	oldPen = SelectObject(windowDC, pen);
	oldBrush = SelectObject(windowDC, brush);
	POINT* points = new POINT[landscapePointsNumber + 2];
	RECT rect;
	::GetClientRect(handle, &rect);
	FillRect(windowDC, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
	double alpha = 0.1;
	points[0].x = 0;
	points[0].y = static_cast<LONG>(landscapeShapeOrdinates[0] * rect.bottom);
	for (unsigned int i = 1; i < landscapePointsNumber; i++) {
		points[i].x = rect.right * i / (landscapePointsNumber - 1);
		points[i].y = static_cast<LONG>(alpha * rect.bottom * landscapeShapeOrdinates[i] + (1 - alpha)
			* points[i - 1].y);
	}
	points[landscapePointsNumber].x = rect.right;
	points[landscapePointsNumber].y = rect.bottom;
	points[landscapePointsNumber + 1].x = rect.left;
	points[landscapePointsNumber + 1].y = rect.bottom;
	Polygon(windowDC, points, landscapePointsNumber + 2);
	SelectObject(windowDC, oldBrush);
	SelectObject(windowDC, oldPen);
	DeleteObject(brush);
	DeleteObject(pen);
	return points;
}

void CGameWindow::drawPartialBar(HDC windowDC, HBRUSH brush, POINT position, int barWidth, int barHeight, double percent)
{
	POINT points[5];
	RECT rect;
	points[0] = position;
	points[0].x -= barWidth / 2;
	points[0].y += barHeight / 2;
	points[1] = points[0];
	points[1].x += barWidth;
	points[2] = points[1];
	points[3] = points[0];
	points[2].y += barHeight;
	points[3].y += barHeight;
	points[4] = points[0];
	Polyline(windowDC, points, 5);
	rect.left = points[0].x;
	rect.right = static_cast<LONG>(points[0].x + (points[1].x - points[0].x) * percent);
	rect.bottom = points[3].y;
	rect.top = points[0].y;
	FillRect(windowDC, &rect, brush);
}

void CGameWindow::drawVelocityBar(HDC windowDC, POINT position, COLORREF color)
{
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HBRUSH brush = CreateSolidBrush(color);
	HGDIOBJ oldPen, oldBrush;
	oldPen = SelectObject(windowDC, pen);
	oldBrush = SelectObject(windowDC, brush);
	RECT rect;
	::GetClientRect(handle, &rect);
	int barWidth = rect.right / 25;
	int barHeight = rect.bottom / 50;
	drawPartialBar(windowDC, brush, position, barWidth, barHeight, velocityValue / maxVelocity);
	SelectObject(windowDC, oldBrush);
	SelectObject(windowDC, oldPen);
	DeleteObject(brush);
	DeleteObject(pen);
}

void CGameWindow::drawHPBar(HDC windowDC, POINT position, COLORREF color, int player) {
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HBRUSH brush = CreateSolidBrush(color);
	HGDIOBJ oldPen, oldBrush;
	oldPen = SelectObject(windowDC, pen);
	oldBrush = SelectObject(windowDC, brush);
	RECT rect;
	::GetClientRect(handle, &rect);
	int barWidth = rect.right / 10;
	int barHeight = rect.bottom / 50;
	drawPartialBar(windowDC, brush, position, barWidth, barHeight, playerHealth[player]);
	SelectObject(windowDC, oldBrush);
	SelectObject(windowDC, oldPen);
	DeleteObject(brush);
	DeleteObject(pen);
}

void CGameWindow::OnPaint() 
{
	PAINTSTRUCT paintStruct;
	RECT clientRect;
	HDC windowDC = ::BeginPaint(handle, &paintStruct);
	::GetClientRect(handle, &clientRect);
	HDC displayBufferDC = CreateCompatibleDC(windowDC);
	HBITMAP displayBuffer = CreateCompatibleBitmap(windowDC, clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top);
	HGDIOBJ oldDisplayBuffer = SelectObject(displayBufferDC, displayBuffer);
	POINT* points = drawLandscape(displayBufferDC, RGB(0, 255, 0));
	int firstPoint = 10;
	while (points[firstPoint].y > points[firstPoint - 1].y ||
		points[firstPoint].y > points[firstPoint + 1].y)
		firstPoint++;
	int secondPoint = landscapePointsNumber - 20;
	while (points[secondPoint].y > points[secondPoint - 1].y ||
		points[secondPoint].y > points[secondPoint + 1].y)
		secondPoint--;
	drawTank(displayBufferDC, playerColor[0], points[firstPoint], playerAngle[0], 0);
	POINT position;
	position.y = clientRect.bottom / 10 * 9;
	position.x = clientRect.right / 10;
	drawHPBar(displayBufferDC, position, playerColor[0], 0);
	drawTank(displayBufferDC, playerColor[1], points[secondPoint], playerAngle[1], 1);
	position.x *= 9;
	drawHPBar(displayBufferDC, position, playerColor[1], 1);
	if (timerState != TS_Flight && timerState != TS_Bomb) {
		drawArrow(displayBufferDC, points[curentTurn ? secondPoint : firstPoint], RGB(0, 255, 0));
	}
	switch (timerState) {
		case TS_Velocity: {
			drawVelocityBar(displayBufferDC, points[curentTurn ? secondPoint : firstPoint], RGB(255, 255, 255));
			break;
		}
		case TS_Flight: {
			HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
			HGDIOBJ oldPen, oldBrush;
			oldPen = SelectObject(displayBufferDC, pen);
			oldBrush = SelectObject(displayBufferDC, brush);
			const int r = 3;
			RECT rect;
			::GetClientRect(handle, &rect);
			Ellipse(displayBufferDC, LONG(ballX * rect.right - r), long(ballY * rect.bottom - r), 
				LONG(ballX * rect.right + r), LONG(ballY * rect.bottom + r));
			DeleteObject(brush);
			DeleteObject(pen);
			break;
		}
		case TS_Bomb: 
		{
			HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
			HBRUSH brush = CreateSolidBrush(RGB(255, 255, 0));
			HGDIOBJ oldPen, oldBrush;
			oldPen = SelectObject(displayBufferDC, pen);
			oldBrush = SelectObject(displayBufferDC, brush);
			RECT rect;
			::GetClientRect(handle, &rect);
			Ellipse(displayBufferDC, LONG(ballX * rect.right - rect.bottom * bombRadius), LONG((ballY - bombRadius) * rect.bottom),
				LONG(ballX * rect.right + rect.bottom * bombRadius), LONG((ballY + bombRadius) * rect.bottom));
			DeleteObject(brush);
			DeleteObject(pen);
			break;
		}
		case TS_GameOver: 
		{
			DrawText(displayBufferDC, L"Game Over", -1, &clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			break;
		}
	}
	for (int i = 0; i < landscapePointsNumber; i++) {
		landscape[i].first = double(points[i].x) / clientRect.right;
		landscape[i].second = double(points[i].y) / clientRect.bottom;
	}
	delete[] points;
	BitBlt(windowDC, clientRect.left, clientRect.top, clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top, displayBufferDC, 0, 0, SRCCOPY);
	SelectObject(displayBufferDC, oldDisplayBuffer);
	DeleteObject(displayBuffer);
	DeleteDC(displayBufferDC);
	::EndPaint(handle, &paintStruct);
}

void CGameWindow::OnSize()
{
	RECT rect;
	::GetClientRect(handle, &rect);
	InvalidateRect(handle, &rect, FALSE);
}

bool CGameWindow::calcCoorsCheckBoom() {
	const double dt = 0.001;
	ballX += ballVX * dt;
	ballY += ballVY * dt;
	ballVY += gravitation * dt;
	while (activeSegmentNumber < landscape.size() - 1 && ballX > landscape[activeSegmentNumber].first) {
		activeSegmentNumber++;
	}
	while (activeSegmentNumber > 1 && ballX < landscape[activeSegmentNumber - 1].first) {
		activeSegmentNumber--;
	}
	double x1 = landscape[activeSegmentNumber].first - landscape[activeSegmentNumber - 1].first;
	double y1 = landscape[activeSegmentNumber].second - landscape[activeSegmentNumber - 1].second;
	double x2 = ballX - landscape[activeSegmentNumber].first;
	double y2 = ballY - landscape[activeSegmentNumber].second;
	return (x1 * y2 - x2 * y1 >= 0);
}

double CGameWindow::eucledianDistance(double x1, double y1, double x2, double y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void CGameWindow::OnTimer()
{
	switch (timerState) {
		case TS_Velocity:
		{
			velocityValue = min(deltaVelocity + velocityValue, maxVelocity);
			break;
		}
		case TS_Flight:
		{
			if (ballX < 0 || ballX > 1) {
				curentTurn = 1 - curentTurn;
				timerState = TS_Inactive;
			} else if (calcCoorsCheckBoom()) {
				timerState = TS_Bomb;
				bombRadius = 0;
			}
			break;
		}
		case TS_Bomb:
		{
			bombRadius += maxRadius / 20;
			if (bombRadius >= maxRadius) {
				for (int i = 0; i < 2; i++) {
					double distance = eucledianDistance(playerCenter[i].first, playerCenter[i].second, ballX, ballY);
					if (distance < maxRadius) {
						playerHealth[i] -= maxDamage * (1 - distance / maxRadius);
						playerHealth[i] = max(0, playerHealth[i]);
					}
				}
				if (playerHealth[0] == 0 || playerHealth[1] == 0) {
					timerState = TS_GameOver;
				} else {
					timerState = TS_Inactive;
					curentTurn = 1 - curentTurn;
				}
			}
		}
	}
	RECT rect;
	::GetClientRect(handle, &rect);
	InvalidateRect(handle, &rect, FALSE);
}

void CGameWindow::OnDestroy() 
{
	KillTimer(handle, timerId);
}

void CGameWindow::OnKeydown(WPARAM wParam, LPARAM lParam) {
	RECT rect;
	::GetClientRect(handle, &rect);
	if (timerState == TS_Inactive) {
		switch (wParam) {
			case VK_UP:
			{
				playerAngle[curentTurn] -= deltaAngle * (curentTurn * 2 - 1);
				InvalidateRect(handle, &rect, FALSE);
				break;
			}
			case VK_DOWN:
			{
				playerAngle[curentTurn] += deltaAngle * (curentTurn * 2 - 1);
				InvalidateRect(handle, &rect, FALSE);
				break;
			}
			case VK_SPACE:
			{
				if ((lParam >> 30) % 2 == 0) {
					timerState = TS_Velocity;
					velocityValue = 0;
				}
				break;
			}
		}
	}
}

void CGameWindow::OnKeyup(WPARAM wParam, LPARAM lParam) {
	if (timerState == TS_Velocity && wParam == VK_SPACE) {
		flightTime = 0;
		timerState = TS_Flight;
		ballX = playerBarrel[curentTurn].first;
		ballY = playerBarrel[curentTurn].second;
		ballVX = std::cos(playerAngle[curentTurn]) * velocityValue;
		ballVY = -std::sin(playerAngle[curentTurn]) * velocityValue;
		int i = 0;
		while (landscape[i].first < ballX) {
			i++;
		}
		activeSegmentNumber = i;
	}
}

LRESULT CGameWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE) {
		CGameWindow* window =
			reinterpret_cast<CGameWindow*>((reinterpret_cast<CREATESTRUCT*>(lParam))->lpCreateParams);
		SetLastError(0);
		SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)window);
		if (GetLastError() != 0) {
			return GetLastError();
		}
		window->OnNCCreate(handle);
		return DefWindowProc(handle, message, wParam, lParam);
	}
	CGameWindow* window = reinterpret_cast<CGameWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	switch (message) {
		case WM_CREATE:
		{
			window->OnCreate();
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_PAINT:
		{
			window->OnPaint();
			return 0;
		}
		case WM_SIZE:
		{
			window->OnSize();
			return 0;
		}
		case WM_DESTROY:
		{
			window->OnDestroy();
			return 0;
		}
		case WM_TIMER:
		{
			window->OnTimer();
			return 0;
		}
		default:
		{
			return DefWindowProc(handle, message, wParam, lParam);
		}
	}
}