#include "windows.h"
#include "thread"
#include "atlimage.h"



double XEquation(double y);
double YEquation(double x);
void task();

void Rotate(int currentAngle);
void PaintAndUpdate();




POINT dcOffset = { 0, 0 };
//TL
//TR
//BL
POINT plgShape[3] = { { 0, 0 },{ 100,0 },{ 0, 100 } };
POINT plgShapeOriginal[3] = { { 0, 0 },{ 100,0 },{ 0, 100 } };
SIZE size = { 0, 0 };
BLENDFUNCTION bf;
enum Directions { LEFT, RIGHT, DOWN, UP };
Directions currentDirection = RIGHT;
int direction = 1;
int x, y;

CImage img;
HWND hWnd;
HDC hdcScreen, hdc;
int width, height = 0;

#define TRANSPARENT_MASK RGB(255,255,255)


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{

	LPCTSTR szWindowClass = _T("TransparentClass");

	// Register class
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = szWindowClass;

	RegisterClassEx(&wcex);

	hWnd = CreateWindowEx(
		GM_ADVANCED | WS_EX_LAYERED | WS_EX_TOPMOST,
		szWindowClass, 0, 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 
		CW_USEDEFAULT, 0, NULL, 
		NULL, hInstance, NULL);
	ShowWindow(hWnd, SW_SHOW);


	hdcScreen = GetDC(0);
	auto modeResullt = SetGraphicsMode(hdcScreen, GM_ADVANCED);
	hdc = CreateCompatibleDC(hdcScreen);


	img.Load(L"122017717912.png");
	//img.Load(L"test2.png");
	//img.Load(L"test3.png");
	//img.Load(L"test4.png");
	width = img.GetWidth();
	height = img.GetHeight();
	HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, width, height);

	
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdc, hBmp);

	size = { width, height };

	plgShape[0] = { 0, 0 };
	plgShape[1] = { width, 0 };
	plgShape[2] =  { 0, height };

	plgShapeOriginal[0] = { 0, 0 };
	plgShapeOriginal[1] = { width, 0 };
	plgShapeOriginal[2] = { 0, height };



	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;



	x = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (width / 2);
	y = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (height / 2);

	PaintAndUpdate();




	std::thread t1(task);
	t1.detach();

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	return (int)msg.wParam;
}


double XEquation(double y) {
	auto x = sqrt((y / 0.001));
	return x >= 5 ? 5 : x;
}
double YEquation(double x) {
	auto y = 0.001*(x*x);
	return y >= 5 ? 5 : y;
}




void task() {
	Sleep(1000);
	
	double tempYLocation = 0;
	double tempXLocation = 0;
	int counter = 0;
	auto currentAngle = 0;
	auto finishedTurn = true;
	auto startedTurn = false;
	auto displacement = 0;
	bool flagRotate = false;



	int lastAngle = 0;
	while (true) {
		Sleep(10);



		if (finishedTurn == false || startedTurn == true) {
			if (startedTurn) {
				displacement = 0;
				startedTurn = false;
				finishedTurn = false;
			}
			if (currentDirection == UP || currentDirection == DOWN) {
				tempYLocation = YEquation(displacement);
				y = y + tempYLocation*direction;
				x = x - (5 - tempYLocation)*direction;
				if (5 - tempYLocation == 0) {
					finishedTurn = true;
					flagRotate = false;
				}
				else {
					Rotate(currentAngle + displacement * 90 / 70);
				}
				displacement++;
			}
			else if (currentDirection == LEFT || currentDirection == RIGHT) {
				tempXLocation = YEquation(displacement);
				x = x + tempXLocation*direction;
				y = y + (5 - tempXLocation)*direction;
				if (5 - tempXLocation == 0) {
					finishedTurn = true;
					flagRotate = false;
				}
				else {
					Rotate(currentAngle + displacement * 90 / 70);
				}
				displacement++;
			}

		}
		else {
			if (currentDirection == LEFT || currentDirection == RIGHT)
			{
				x = x + (5 * direction);
			}
			else if (currentDirection == UP || currentDirection == DOWN) {
				y = y + (5 * direction);
			}
			if (x >= (GetSystemMetrics(SM_CXSCREEN)) - width - 300 && currentDirection == RIGHT) {
				currentDirection = UP;
				direction = -1;
				startedTurn = true;
				flagRotate = true;
				currentAngle = 0;
			}
			else if (y <= 300 && currentDirection == UP) {
				currentDirection = LEFT;
				startedTurn = true;
				flagRotate = true;
				direction = -1;
				currentAngle = 90;
			}
			else if (x <= 300 && currentDirection == LEFT) {
				currentDirection = DOWN;
				direction = 1;
				startedTurn = true;
				flagRotate = true;
				currentAngle = 180;
			}
			else if (y >= (GetSystemMetrics(SM_CYSCREEN)) - height - 300 && currentDirection == DOWN) {
				currentDirection = RIGHT;
				direction = 1;
				startedTurn = true;
				flagRotate = true;
				currentAngle = 270;
			}
		}


		PaintAndUpdate();
	}
}




void Rotate(int currentAngle) {

	auto newAngle = -currentAngle * 3.1415926535897 / 180.0;
	float s = sin(newAngle);
	float c = cos(newAngle);


	for (auto i = 0; i < 3; i++) {
		auto newPointX = plgShapeOriginal[i].x - width / 2;
		auto newPointY = plgShapeOriginal[i].y - height / 2;

		LONG finalPointX = newPointX*c - newPointY*s;
		LONG finalPointY = newPointX*s + newPointY*c;

		finalPointX = finalPointX + width / 2;
		finalPointY = finalPointY + height / 2;

		plgShape[i] = { finalPointX , finalPointY };
	}
}


void PaintAndUpdate() {
	auto test2 = img.PlgBlt(hdc, plgShape, 0);
	POINT newPoint = { x,y };
	auto layerResult2 = UpdateLayeredWindow(hWnd, hdcScreen, &newPoint, &size, hdc, &dcOffset, TRANSPARENT_MASK, &bf, ULW_COLORKEY);
}