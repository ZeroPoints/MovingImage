#include "windows.h"
#include "resource.h"
#include "thread"
#include <iostream>
#include <exception>
#include "cmath"
#include "atlimage.h"




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
		WS_EX_LAYERED | WS_EX_TOPMOST, 
		szWindowClass, 0, 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 
		CW_USEDEFAULT, 0, NULL, 
		NULL, hInstance, NULL);


	int width, height = 0;
	HDC hdcScreen = GetDC(0);
	HDC hdc = CreateCompatibleDC(hdcScreen);


	CImage img;
	img.Load(L"test.png");
	width = img.GetWidth();
	height = img.GetHeight();
	HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, width, height);

	
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdc, hBmp);


	POINT dcOffset = { 0, 0 };
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
	//auto test2 = img.PlgBlt(hdcScreen, &dcOffset, 0);
	//auto test1 = PlgBlt(hdcScreen, &dcOffset, hdc, 0, 0, width, height, 0, 0, 0);


	auto layerResult = UpdateLayeredWindow(hWnd, hdcScreen, 0, &size, hdc, &dcOffset, 0, &bf, 0);


	ShowWindow(hWnd, SW_SHOW);





	DeleteDC(hdc);
	DeleteObject(hBmp);


	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}







