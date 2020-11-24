#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>

#include <memory>
#include <time.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <iostream>

#include "InversusFramework.h"
#include "ClientSocketManager.h"
#include "PaintInfo.h"
#include "Vec2D.h"
#include "resource.h"

#ifdef UNICODE
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif


//#define PEEKMESSAGE

#define UM_TIMER 5124124

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"S";
LPCTSTR lpszWindowName = L"Inversus";

constexpr size_t X_SIZE = 1000;
constexpr size_t Y_SIZE = 820;

constexpr int interval = 16;
//constexpr int interval = 33;

LRESULT CALLBACK wProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

LONGLONG updatecnt = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR ipszCmdParam, int mCmdShow)
{
	static bool timeStart = false;
	static LARGE_INTEGER Frequency;
	static LARGE_INTEGER BeginTime;
	static LARGE_INTEGER Endtime;

	std::ifstream in( "serverAddr.txt" );
	std::string addr( std::istream_iterator<char>( in ), std::istream_iterator<char>{} );
	std::wstring temp( addr.begin(), addr.end() );
	MessageBox( NULL, temp.c_str(), L"", MB_OK );
	ClientSocketManager::GetInstance()->InitalizeWinSock();
	ClientSocketManager::GetInstance()->CreateSocket();
	ClientSocketManager::GetInstance()->SetConnection( addr.c_str() );
	ClientSocketManager::GetInstance()->StartRecvThread();

	HWND hWnd;
	MSG Message = MSG();
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)wProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_CATDID));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassEx(&WndClass);
	srand(time(NULL));
	hWnd = CreateWindow(
		lpszClass, lpszWindowName,
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_MINIMIZEBOX ^ WS_THICKFRAME | WS_BORDER,
		0, 0, X_SIZE, Y_SIZE,
		NULL, (HMENU)NULL,
		hInstance, NULL
	);
	ShowWindow(hWnd, mCmdShow);
	UpdateWindow(hWnd);
	//SetTimer(hWnd, UM_TIMER, 20, nullptr);
#ifdef PEEKMESSAGE
	while (WM_QUIT != Message.message)
	{
		// 메세지큐에 메세지가 있는지 검사한다.  
		// 메세지가 있건,없건 그냥 리턴한다.  
		if (PeekMessage(&Message, 0, 0, 0, PM_NOREMOVE))
		{
			//메세지 큐에서 메세지를 가져온다.  
			GetMessage(&Message, 0, 0, 0);
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			if (timeStart == false)
			{
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BeginTime);
				QueryPerformanceCounter(&Endtime);
				timeStart = true;
			}

			QueryPerformanceCounter(&Endtime);
			INT64 elapsed = Endtime.QuadPart - BeginTime.QuadPart;
			double duringtime = (double)elapsed / (double)Frequency.QuadPart;

			framework.Update(static_cast<float>(duringtime));
			updatecnt++;
			QueryPerformanceCounter(&BeginTime);
			InvalidateRect(hWnd, nullptr, false);
		}
	}
#else
	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
#endif // DEBUG
	std::cout << "Destroy" << std::endl;
	ClientSocketManager::GetInstance()->CloseConnection();
	ClientSocketManager::GetInstance()->Destroy();
	return Message.wParam;
}





LRESULT CALLBACK wProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDc;
	static HDC bufferDc;
	static HBITMAP OldBufferBit;
	static HBITMAP hBufferBit;

	static HDC StretchDc;
	static HBITMAP OldStretchBit;
	static HBITMAP hStretchBit;

	static HDC ImageDc;

	static int sizeX = 0;
	static int sizeY = 0;
	static POINT center = { X_SIZE / 2 , Y_SIZE / 2 };
	static RECT ClientRect = {};
	static SIZE textSize = {};
	static LPCTSTR printChar = L"Game Clear!";

	static Vec2DU Margin = { 50,50 };

	static LARGE_INTEGER Frequency;
	static LARGE_INTEGER BeginTime;
	static LARGE_INTEGER Endtime;
	static bool timeStart = false;

	static bool isClicked = false;

	switch (iMessage)
	{
	case WM_CREATE:
	{
		InversusFramework::GetInstance()->Create();

		hDc = GetDC(hWnd);

		HFONT font = CreateFont(90, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"궁서");
		auto oldfont = SelectObject(hDc, font);
		GetTextExtentPoint(hDc, printChar, lstrlenW(printChar), &textSize);
		SelectObject(hDc, oldfont);
		DeleteObject(font);

		GetClientRect(hWnd, &ClientRect);
		sizeX = ClientRect.right - ClientRect.left;
		sizeY = ClientRect.bottom - ClientRect.top;
#ifdef PEEKMESSAGE
		//SetTimer(hWnd, UM_TIMER, interval, nullptr);
#else
		SetTimer(hWnd, UM_TIMER, interval, nullptr);
#endif // PEEKMESSAGE

		ReleaseDC(hWnd, hDc);
	}
	break;
	case WM_DESTROY:
	{
		std::cout << "Destroy" << std::endl;
		ClientSocketManager::GetInstance()->CloseConnection();
		ClientSocketManager::GetInstance()->Destroy();
		PostQuitMessage(0);
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		InversusFramework::GetInstance()->MouseInput( Vec2DU{ xPos,yPos }, iMessage );
	}
	break;
	case WM_MOUSEMOVE:
	{
		int xPos = LOWORD( lParam );
		int yPos = HIWORD( lParam );
		InversusFramework::GetInstance()->MouseInput( Vec2DU{ xPos,yPos }, iMessage );
	}
	break;
	case WM_LBUTTONUP:
	{
		int xPos = LOWORD( lParam );
		int yPos = HIWORD( lParam );
		InversusFramework::GetInstance()->MouseInput(Vec2DU{ xPos,yPos }, iMessage);
	}
	break;
	case WM_TIMER:
	{
		switch (wParam)
		{
		case UM_TIMER:
		{
#ifdef PEEKMESSAGE
			InvalidateRect(hWnd, nullptr, false);
#else
			if (timeStart == false)
			{
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BeginTime);
				QueryPerformanceCounter(&Endtime);
				timeStart = true;
			}

			QueryPerformanceCounter(&Endtime);
			INT64 elapsed = Endtime.QuadPart - BeginTime.QuadPart;
			double duringtime = (double)elapsed / (double)Frequency.QuadPart;

			InversusFramework::GetInstance()->Update(static_cast<float>(duringtime));

			QueryPerformanceCounter(&BeginTime);
			InvalidateRect(hWnd, nullptr, false);
#endif // PEEKMESSAGE

		}
		break;
		default:
			break;

		}
	}
	break;
	case WM_PAINT:
	{

		float AntiAliasing = 1.0f;
		//float AntiAliasing = 3.0f / 2;
		//39 *  21
		hDc = BeginPaint(hWnd, &ps);
		ImageDc = CreateCompatibleDC(hDc);
		bufferDc = CreateCompatibleDC(hDc);
		StretchDc = CreateCompatibleDC(hDc);
		if (hBufferBit == NULL)
		{
			hBufferBit = CreateCompatibleBitmap(hDc, (ClientRect.right * AntiAliasing + Margin.x * 2), (ClientRect.bottom * AntiAliasing + Margin.x * 2 * 2));
		}
		if (hStretchBit == NULL)
		{
			hStretchBit = CreateCompatibleBitmap(hDc, X_SIZE, Y_SIZE);
		}
		OldBufferBit = (HBITMAP)SelectObject(bufferDc, hBufferBit);
		OldStretchBit = (HBITMAP)SelectObject(StretchDc, hStretchBit);
		RECT BufferRect = { 0,0,(ClientRect.right * AntiAliasing + Margin.x * 2),(ClientRect.bottom * AntiAliasing + Margin.y * 2) };
		FillRect(bufferDc, &BufferRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		FillRect(StretchDc, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		//FillRect(hDc, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

		PaintInfo pInfo;
		pInfo.hinst = g_hInst;
		pInfo.hdc = bufferDc;
		pInfo.imageDc = ImageDc;
		pInfo.stretchDc = StretchDc;
		pInfo.margin = Margin;
		pInfo.DrawSize = Vec2DU{ClientRect.right,ClientRect.bottom};
		pInfo.AntiAliasing = AntiAliasing;

		InversusFramework::GetInstance()->Draw(pInfo);

		SetStretchBltMode(StretchDc, COLORONCOLOR);
		//SetStretchBltMode(hDc, COLORONCOLOR);
		auto displaySize = InversusFramework::GetInstance()->GetDisplaySize();


		StretchBlt(StretchDc, 10, 10, displaySize.x, displaySize.y,
			bufferDc, Margin.x + InversusFramework::GetInstance()->GetMargin().x* AntiAliasing, Margin.y + InversusFramework::GetInstance()->GetMargin().y* AntiAliasing, displaySize.x* AntiAliasing, displaySize.y* AntiAliasing,
			SRCCOPY);

		pInfo.hdc = StretchDc;
		InversusFramework::GetInstance()->UIDraw(pInfo);
		InversusFramework::GetInstance()->MenuDraw(pInfo);


		BitBlt(hDc, 0, 0, ClientRect.right, ClientRect.bottom, StretchDc, 0, 0, SRCCOPY);
		SelectObject(bufferDc, OldBufferBit);
		SelectObject(StretchDc, OldStretchBit);
		DeleteDC(StretchDc);
		DeleteDC(bufferDc);
		DeleteDC(ImageDc);
		EndPaint(hWnd, &ps);
	}
	break;

	}
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}