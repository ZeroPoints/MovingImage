#include "windows.h"
#include "resource.h"
#include "thread"
#include <iostream>
#include <exception>
#include "cmath"



#define MAX_LOADSTRING 100


#pragma region Funcs

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK DialogProc(
	HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
);

void createRegion(HWND);
void paintRegion();
void TransformWorldRotate(double angle);
double XEquation(double y);
double YEquation(double x);
void task();
void BuildRegion();
void RotateRegion(double angle);
XFORM GetTransformMatrixDC(double angle);
XFORM GetTransformMatrixRegion(double angle);

#pragma endregion



#pragma region Vars


HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];						
HRGN hRgn = NULL;
HDC hdcDest, hdcMem;
BITMAP bmpInfo;
double currentAngle = 0;
auto displacement = 0;
auto finishedTurn = true;
auto startedTurn = false;
bool flagRotate = false;
enum Directions { LEFT, RIGHT, DOWN, UP };
Directions currentDirection = RIGHT;
int direction = 1;
int x, y;
HGDIOBJ hGdiObj;
double previousAngle = 0;
HWND hwndDlgsource = NULL;
HWND ghWnd = NULL;
BOOL mouseState = 0;
POINT CursPt = { 0 };

#pragma endregion





int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	x = 0;
	y = 0;
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	std::thread t1(task);
	t1.detach();
	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
	return msg.wParam;
}






double XEquation(double y) {
	auto x = sqrt((y/0.001));
	return x >= 5 ? 5 : x;
}
double YEquation(double x) {
	auto y = 0.001*(x*x);
	return y >= 5 ? 5 : y;
}





void task() {
	Sleep(1000);
	auto x = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (bmpInfo.bmWidth / 2);
	auto y = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (bmpInfo.bmHeight / 2);
	double tempYLocation = 0;
	double tempXLocation = 0;
	int counter = 0;

	int lastAngle = 0;
	while (true) {
		Sleep(10);


		counter++;
		if (counter % 30 == 0) {
			lastAngle = lastAngle + 15;
			RotateRegion(lastAngle);
		}

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
				displacement++;
				TransformWorldRotate(currentAngle + displacement * 90 / 71);
			}
			else if (currentDirection == LEFT || currentDirection == RIGHT) {
				tempXLocation = YEquation(displacement);
				x = x + tempXLocation*direction;
				y = y + (5 - tempXLocation)*direction;
				if (5 - tempXLocation == 0) {
					finishedTurn = true;
					flagRotate = false;
				}
				displacement++;
				TransformWorldRotate(currentAngle + displacement * 90 / 71);
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
			if (x >= (GetSystemMetrics(SM_CXSCREEN)) - bmpInfo.bmWidth - 300 && currentDirection == RIGHT) {
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
			else if (y >= (GetSystemMetrics(SM_CYSCREEN)) - bmpInfo.bmHeight - 300 && currentDirection == DOWN) {
				currentDirection = RIGHT;
				direction = 1;
				startedTurn = true;
				flagRotate = true;
				currentAngle = 270;
			}
		}
		MoveWindow(hwndDlgsource, x, y, 354, 354, true);
		paintRegion();
	}
}




void createRegion(HWND hwndDlg)
{
	hdcDest = GetDC(hwndDlg);
	SetGraphicsMode(hdcDest, GM_ADVANCED);
	hdcMem = CreateCompatibleDC(NULL);
	HANDLE hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("122017717912.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hBitmap, sizeof(bmpInfo), &bmpInfo);	
	hGdiObj = SelectObject(hdcMem, hBitmap);
	BuildRegion();
	auto iX = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (bmpInfo.bmWidth / 2);
	auto iY = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (bmpInfo.bmHeight / 2);
	auto iRet = SetWindowPos(hwndDlg, HWND_TOPMOST, iX, iY, 354, 354, NULL);
	paintRegion();
	DeleteObject(hBitmap);
}





void paintRegion()
{
	BitBlt(hdcDest, 0, 0, 354, 354, hdcMem, 0, 0, SRCCOPY);
}




void TransformWorldRotate(double angle) {
	auto matrix = GetTransformMatrixDC(angle);
	auto tmp = SetWorldTransform(hdcDest, &matrix);
	//RotateRegion(angle);
}




void BuildRegion() {
	hRgn = CreateRectRgn(0, 0, 0, 0);
	COLORREF crTransparent = RGB(255, 255, 255);
	for (int iY = 0; iY < bmpInfo.bmHeight; iY++)
	{
		for(int iX = 0; iX < bmpInfo.bmWidth; iX++)
		{
			if (GetPixel(hdcMem, iX, iY) != crTransparent) {
				HRGN hRgnTemp = CreateRectRgn(iX, iY, iX + 1, iY + 1);
				auto iRety = CombineRgn(hRgn, hRgn, hRgnTemp, RGN_OR);
				DeleteObject(hRgnTemp);
			}
		}  
	}
	auto iRet = SetWindowRgn(hwndDlgsource, hRgn, TRUE);
}


void RotateRegion(double angle) {
	auto tmpAngle = angle - previousAngle;
	previousAngle = angle;
	auto matrix = GetTransformMatrixRegion(tmpAngle);
	HRGN newRGN = CreateRectRgn(0, 0, 0, 0);
	auto complexity = GetWindowRgn(hwndDlgsource, newRGN);
	int iSize = GetRegionData(newRGN, sizeof(RGNDATA), NULL);
	RGNDATA* pData = (RGNDATA*)calloc(iSize, 1);
	pData->rdh.dwSize = iSize;
	int iSize2 = GetRegionData(newRGN, iSize, pData);
	auto finalRgn = ExtCreateRegion(&matrix, iSize, pData);
	auto iRet = SetWindowRgn(hwndDlgsource, finalRgn, TRUE);

	delete(pData);
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



XFORM GetTransformMatrixRegion(double angle) {










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














































ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("TEST");
	wcex.hIconSm = NULL;
	return RegisterClassEx(&wcex);
}
















BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int iRet = 0;
	hInst = hInstance;
	ghWnd = CreateWindow(
		TEXT("TEST"),
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		354,
		354,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (!ghWnd)
	{
		return FALSE;
	}
	hwndDlgsource = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), ghWnd, (DLGPROC)DialogProc);
	createRegion(hwndDlgsource);
	return TRUE;
}








LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		DeleteObject(hRgn);
		DeleteDC(hdcMem);
		DeleteDC(hdcDest);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}










INT_PTR CALLBACK DialogProc(
	HWND hwnd,  
	UINT uMsg,  
	WPARAM wParam, 
	LPARAM lParam  
)
{
	int ix = 0, iy = 0;
	switch (uMsg)
	{
	case WM_PAINT:
		paintRegion();
		ValidateRect(hwnd, FALSE);
		return TRUE;
	case WM_MOVE:
		return TRUE;
	case WM_LBUTTONDOWN:
		mouseState = 1;
		return TRUE;
	case WM_MOUSEMOVE:
		if (mouseState)
		{
			GetCursorPos(&CursPt);
			PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(CursPt.x, CursPt.y));
			mouseState = 0;
		}
		return TRUE;
	//case WM_RBUTTONDOWN:
	//	EndDialog(hwnd, 0);
	//	//send message to destroy the main application window and exit
	//	SendMessage(ghWnd, WM_DESTROY, NULL, NULL);
	//	return TRUE;
	}
	return FALSE;
}

