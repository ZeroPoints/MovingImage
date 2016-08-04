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
	img.Load(L"test3.png");
	width = img.GetWidth();
	height = img.GetHeight();
	HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, width, height);

	
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdc, hBmp);


	POINT dcOffset = { 0, 0 };
	POINT dcOffsetTest[3] = { { 0, 0 },{ 0,height },{ height, 0 } };
	//POINT dcOffsetTest[3] = { { 0, 0 }, { 0, 0 },{ 0, 0} };
	POINT dcOffset2 = { 0, 0 };
	SIZE size = { width, height };
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;


	#define TRANSPARENT_MASK RGB(255,255,255)


	//auto test5 = img.TransparentBlt(hdc, 0, 0, width, height, TRANSPARENT_MASK);
	//auto test4 = img.Draw(hdc, 0, 0, width, height, 0, 0, width, height);
	//auto test3 = img.PlgBlt(hdcScreen, &dcOffset, 0, 0, width, height, 0, 0, 0);
	auto test2 = img.PlgBlt(hdc, dcOffsetTest, 0);
	//auto test1 = PlgBlt(hdcScreen, &dcOffset, hdc, 0, 0, width, height, 0, 0, 0);



	//auto newAngle = 90 * 3.1415926535897 / 180.0;
	//float cosine = (float)cos(newAngle);
	//float sine = (float)sin(newAngle);
	//XFORM transform;
	//transform.eM11 = (FLOAT)cos(newAngle);
	//transform.eM12 = (FLOAT)(-sin(newAngle));
	//transform.eM21 = (FLOAT)(sin(newAngle));
	//transform.eM22 = (FLOAT)cos(newAngle);
	//transform.eDx = 0;
	//transform.eDy = 0;
	//XFORM transform2;
	//transform2.eM11 = 1;
	//transform2.eM12 = 0;
	//transform2.eM21 = 0;
	//transform2.eM22 = 1;
	//transform2.eDx = -width / 2;
	//transform2.eDy = -height / 2;
	//XFORM transform3;
	//transform3.eM11 = 1;
	//transform3.eM12 = 0;
	//transform3.eM21 = 0;
	//transform3.eM22 = 1;
	//transform3.eDx = width / 2;
	//transform3.eDy = height / 2;
	//XFORM result1;
	//XFORM result2;
	//CombineTransform(&result1, &transform2, &transform);
	//CombineTransform(&result2, &result1, &transform3);

	//int currentAngle = 50;
	//auto resultMatrix = GetTransformMatrixDC(currentAngle);
	//auto tmp = SetWorldTransform(hdcScreen, &resultMatrix);
	//currentAngle++;

	auto layerResult = UpdateLayeredWindow(hWnd, hdcScreen, 0, &size, hdc, &dcOffset, TRANSPARENT_MASK, &bf, ULW_ALPHA);

	//while (true) {
	//	Sleep(100);
	//	auto layerResult2 = UpdateLayeredWindow(hWnd, hdcScreen, &dcOffset2, &size, hdc, &dcOffset, 0, &bf, ULW_ALPHA);

	//	dcOffset2.x++;
	//	dcOffset2.y++;

	//}


	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	return (int)msg.wParam;
}




XFORM GetTransformMatrixDC(double angle) {
	auto newAngle = angle * 3.1415926535897 / 180.0;
	float cosine = (float)cos(newAngle);
	float sine = (float)sin(newAngle);
	XFORM transform;
	transform.eM11 = (FLOAT)cos(newAngle);
	transform.eM12 = (FLOAT)(-sin(newAngle));
	transform.eM21 = (FLOAT)(sin(newAngle));
	transform.eM22 = (FLOAT)cos(newAngle);
	transform.eDx = 0;
	transform.eDy = 0;
	XFORM transform2;
	transform2.eM11 = 1;
	transform2.eM12 = 0;
	transform2.eM21 = 0;
	transform2.eM22 = 1;
	transform2.eDx = -354 / 2;
	transform2.eDy = -354 / 2;
	XFORM transform3;
	transform3.eM11 = 1;
	transform3.eM12 = 0;
	transform3.eM21 = 0;
	transform3.eM22 = 1;
	transform3.eDx = 354 / 2;
	transform3.eDy = 354 / 2;
	XFORM result1;
	XFORM result2;
	CombineTransform(&result1, &transform2, &transform);
	CombineTransform(&result2, &result1, &transform3);
	return result2;
}




