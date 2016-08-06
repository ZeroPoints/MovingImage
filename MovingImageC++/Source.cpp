#include "windows.h"
#include "resource.h"
#include "thread"
#include <iostream>
#include <exception>
#include "cmath"
#include "atlimage.h"


XFORM GetTransformMatrixDC(double angle);





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

	HWND hWnd = CreateWindowEx(
		GM_ADVANCED | WS_EX_LAYERED | WS_EX_TOPMOST,
		szWindowClass, 0, 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 
		CW_USEDEFAULT, 0, NULL, 
		NULL, hInstance, NULL);
	ShowWindow(hWnd, SW_SHOW);


	int width, height = 0;
	HDC hdcScreen = GetDC(0);
	auto modeResullt = SetGraphicsMode(hdcScreen, GM_ADVANCED);
	HDC hdc = CreateCompatibleDC(hdcScreen);


	CImage img;
	img.Load(L"test2.png");
	//img.Load(L"test3.png");
	//img.Load(L"test4.png");
	width = img.GetWidth();
	height = img.GetHeight();
	HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, width, height);

	
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdc, hBmp);


	POINT dcOffset = { 0, 0 };
	//TL
	//TR
	//BL
	POINT dcOffset2[3] = { { 0, 0 },{ 100,0 },{ 0, 100 } };
	POINT dcOffset2Original[3] = { { 0, 0 },{ 100,0 },{ 0, 100 } };
	SIZE size = { width, height };
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;


	#define TRANSPARENT_MASK RGB(255,255,255)


	auto test2 = img.PlgBlt(hdc, dcOffset2, 0);


	auto layerResult = UpdateLayeredWindow(hWnd, hdcScreen, 0, &size, hdc, &dcOffset, TRANSPARENT_MASK, &bf, ULW_ALPHA | ULW_COLORKEY);


	auto currentAngle = 1;

	while (true) {
		Sleep(10);


		auto newAngle = currentAngle * 3.1415926535897 / 180.0;
		float s = sin(newAngle);
		float c = cos(newAngle);


		auto newPointX = dcOffset2Original[0].x - 50;
		auto newPointY = dcOffset2Original[0].y - 50;

		LONG finalPointX = newPointX*c - newPointY*s;
		LONG finalPointY = newPointX*s + newPointY*c;

		finalPointX = finalPointX + 50;
		finalPointY = finalPointY + 50;

		dcOffset2[0] = { finalPointX , finalPointY };

		//------------------------------------------------------
		newPointX = dcOffset2Original[1].x - 50;
		newPointY = dcOffset2Original[1].y - 50;

		finalPointX = newPointX*c - newPointY*s;
		finalPointY = newPointX*s + newPointY*c;

		finalPointX = finalPointX + 50;
		finalPointY = finalPointY + 50;

		dcOffset2[1] = { finalPointX , finalPointY };

		//------------------------------------------------------
		newPointX = dcOffset2Original[2].x - 50;
		newPointY = dcOffset2Original[2].y - 50;

		finalPointX = newPointX*c - newPointY*s;
		finalPointY = newPointX*s + newPointY*c;

		finalPointX = finalPointX + 50;
		finalPointY = finalPointY + 50;

		dcOffset2[2] = { finalPointX , finalPointY };


		auto test2 = img.PlgBlt(hdc, dcOffset2, 0);
		auto layerResult2 = UpdateLayeredWindow(hWnd, hdcScreen, 0, &size, hdc, &dcOffset, TRANSPARENT_MASK, &bf, ULW_ALPHA | ULW_COLORKEY);


		currentAngle++;
		if(currentAngle >= 360){
			currentAngle = 0;
		}

	}


	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	return (int)msg.wParam;
}


