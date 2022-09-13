#include <cstdint>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include "menu.hpp"
#include "Board.h"

static HBITMAP ico1_hBitmap, ico2_hBitmap;
HDC ico1_hBuffer , ico2_hBuffer , wDC , board_tDC , text_tDC;
HWND m_hWnd;
HMENU m_hMenu;
HFONT m_hFont;
HBRUSH m_grayBrush , m_whiteBrush , m_blackBrush , m_greenBrush , m_lightBlueBrush;
Board* m_board;
HBITMAP text_bufferBMP;
int board_x , board_y , board_w , board_s , text_x , text_y , blackCount , whiteCount , DisplayType;
//DisplayType: 0:480x320 1:480x800 2:800x480
//board: x,y:coordinate w:weight s:each space
auto board = m_board->getBoard();

inline void drawText(HDC hDC , const TCHAR *str , RECT rc , UINT format) { DrawText(hDC , str , lstrlen(str) , &rc , format); }
void drawText(HDC hdc , int i , RECT rc , UINT format)
{
	TCHAR buf[10];
	wsprintf(buf , _T("%d") , i);
	drawText(hdc , buf , rc , format);
}

void RefreshBuffer()
{
	RECT board_rc = { 0,0,board_w + 1,board_w + 1};
	RECT text_rc = { 0,0,120,50 };
	FillRect(board_tDC, &board_rc, m_grayBrush);
	FillRect(text_tDC, &text_rc, m_grayBrush);

	// Start rendering
	board = m_board->getBoard();
	blackCount = 0 , whiteCount = 0;
	ico1_hBuffer = CreateCompatibleDC(board_tDC);
	SelectObject(ico1_hBuffer , ico1_hBitmap);
	ico2_hBuffer = CreateCompatibleDC(board_tDC);
	SelectObject(ico2_hBuffer , ico2_hBitmap);

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (board[i][j] == BlockStatus::BLACK)
			{
				SelectObject(board_tDC, m_blackBrush);
				BitBlt (board_tDC, j * board_s, i * board_s, board_s, board_s, ico2_hBuffer, 0, 0, SRCCOPY);
				++blackCount;
			}
			else if(board[i][j] == BlockStatus::WHITE)
			{
				SelectObject(board_tDC, m_whiteBrush);
				BitBlt (board_tDC, j * board_s, i * board_s, board_s, board_s, ico1_hBuffer, 0, 0, SRCCOPY);
				++whiteCount;
			}
		}
	}
	DeleteDC(ico1_hBuffer);
	DeleteDC(ico2_hBuffer);
	SelectObject(board_tDC, m_lightBlueBrush);
	if(DisplayType == 0)
	{
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{ if (board[i][j] == BlockStatus::AVAILABLE) { Ellipse(board_tDC, j * 30 + 8, i * 30 + 8, j * 30 + 23, i * 30 + 23); } }
		}
	}
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{ if (board[i][j] == BlockStatus::AVAILABLE) { Ellipse(board_tDC, j * 50 + 15, i * 50 + 15, j * 50 + 35, i * 50 + 35); } }
		}
	}

	for (int i = 0; i < 9; ++i) { MoveToEx(board_tDC, i * board_s , 0 , NULL) , LineTo(board_tDC, i * board_s, board_w + 1); }
	for (int i = 0; i < 9; ++i) { MoveToEx(board_tDC, 0, i * board_s, NULL) , LineTo(board_tDC, board_w + 1, i * board_s); }

	SelectObject(text_tDC, m_hFont);
	SetBkMode(text_tDC, TRANSPARENT);
	SetTextColor(text_tDC, RGB(0, 0, 0));
	drawText(text_tDC, blackCount, { 0,0,55,50 }, DT_CENTER);
	SetTextColor(text_tDC, RGB(0, 255, 0));
	drawText(text_tDC, _T(" : "), { 55,0,65,50 }, DT_CENTER);
	SetTextColor(text_tDC, RGB(255, 255, 255));
	drawText(text_tDC, whiteCount, { 65,0,120,50 }, DT_CENTER);
}

inline void DrawBuffer(HDC wDC)
{
	BitBlt(wDC , board_x , board_y , board_w + 1 , board_w + 1 , board_tDC , 0 , 0 , SRCCOPY);
	BitBlt(wDC,  text_x , text_y, 120, 50, text_tDC, 0, 0, SRCCOPY);
}

void RefreshWindow()
{
	RefreshBuffer();
	DrawBuffer(GetDC(m_hWnd));

	if (m_board->isGameOver || blackCount == 0 || whiteCount == 0)
	{
		const auto SHOW_RESULT = [&](const TCHAR *msg) {
			MessageBox(m_hWnd , msg , _T("Reversi") , MB_OK | MB_ICONASTERISK);
			m_board->Set();
			RefreshWindow();
		};
		if (whiteCount < blackCount) { SHOW_RESULT(_T("You Win!")); }
		else if (whiteCount == blackCount) { SHOW_RESULT(_T("Draw!")); }
		else if (whiteCount > blackCount) { SHOW_RESULT(_T("You Lose...\nGenkaiya...")); }
		return;
	}
	else if (m_board->isnoAvailable)
	{
		MessageBox(m_hWnd , TEXT("Since you have no more moves, AI goes again.") , TEXT("Reversi") , MB_OK | MB_ICONEXCLAMATION);
		m_board->robot();
		m_board->setAvailable();
		RefreshWindow();
		return;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	POINTS pos;
	static LOGFONT font;
	static HBITMAP board_bufferBMP;
	static unsigned __int16 AniTime = 200;
	const auto LOAD_BITMAP = [&](const TCHAR *name)
	{
		TCHAR path[MAX_PATH];
		GetModuleFileName(NULL , path , MAX_PATH); // 実行可能ファイルのパスを取得
		*(_tcsrchr(path , _T('\\')) + 1) = _T('\0'); // 実行可能ファイルのディレクトリのみにする
		lstrcat(path , name);
		lstrcat(path , _T(".bmp"));

		#ifdef _WIN32_WCE
		return SHLoadDIBitmap(path);
		#else
		return (HBITMAP)LoadImage (
			((LPCREATESTRUCT)(lParam))->hInstance ,
			path ,
			IMAGE_BITMAP ,
			0 ,
			0 ,
			LR_LOADFROMFILE
		);
		#endif
	};

	#ifdef _WIN32_WCE
	HWND hCommandBar;
	RECT rect;
	#else
	static HCURSOR hand = LoadCursor(NULL, IDC_HAND) , normal = LoadCursor(NULL, IDC_ARROW);
	#endif

	switch (message)
	{
		case WM_LBUTTONDOWN:
			pos = MAKEPOINTS(lParam);
			if ((pos.x >= board_x && pos.x <=board_w + board_x) && (pos.y >= board_y && pos.y <= board_w + board_y))
			{
				// in rect
				pos.x -= board_x , pos.x /= board_s;
				pos.y -= board_y , pos.y /= board_s;
				if (m_board->getBoard()[pos.y][pos.x] != BlockStatus::AVAILABLE) { return 0; }
				m_board->mouseClick(pos.y, pos.x);
				RefreshWindow();
				Sleep(AniTime);
				m_board->robot();
				m_board->setAvailable();
				RefreshWindow();
			}
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case FILE_NEW_GAME :
					m_board->Set();
					RefreshWindow();
					break;
				case FILE_EXIT:
					PostMessage(m_hWnd, WM_CLOSE, 0, 0);
					break;
				case ANIMATION_FAST:
					AniTime = 100;
					CheckMenuItem(m_hMenu, 201, MF_BYCOMMAND | MFS_CHECKED);
					CheckMenuItem(m_hMenu, 202, MF_BYCOMMAND | MFS_UNCHECKED);
					CheckMenuItem(m_hMenu, 203, MF_BYCOMMAND | MFS_UNCHECKED);
					RefreshWindow();
					break;
				case ANIMATION_NORMAL:
					AniTime = 200;
					CheckMenuItem(m_hMenu, 201, MF_BYCOMMAND | MFS_UNCHECKED);
					CheckMenuItem(m_hMenu, 202, MF_BYCOMMAND | MFS_CHECKED);
					CheckMenuItem(m_hMenu, 203, MF_BYCOMMAND | MFS_UNCHECKED);
					RefreshWindow();
					break;
				case ANIMATION_SLOW:
					AniTime = 400;
					CheckMenuItem(m_hMenu, 201, MF_BYCOMMAND | MFS_UNCHECKED);
					CheckMenuItem(m_hMenu, 202, MF_BYCOMMAND | MFS_UNCHECKED);
					CheckMenuItem(m_hMenu, 203, MF_BYCOMMAND | MFS_CHECKED);
					RefreshWindow();
					break;
				case HELP_ABOUT:
					MessageBox (
						m_hWnd ,
						TEXT("Genkaiya Reversi\nVersion 3.0 BETA3\n\nCopylight (c) 2022 TMK, 777shuang. All Righits Reserved.") ,
						TEXT("About") ,
						MB_OK | MB_ICONASTERISK
					);
					break;
			}
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				m_board->Set();
				RefreshWindow();
			}
			break;
		case WM_CREATE:
			#ifdef _WIN32_WCE
			InitCommonControls();
			hCommandBar = CommandBar_Create(((LPCREATESTRUCT)(lParam))->hInstance, hWnd, 8);
			if (hCommandBar == NULL)
			{
				MessageBox(hWnd , _T("cannot create commandbar."), _T("Reversi"), MB_OK | MB_ICONERROR);
			}
			CommandBar_InsertMenubarEx(hCommandBar , ((LPCREATESTRUCT)(lParam))->hInstance , _T("WNDMENU") , 8);
			CommandBar_AddAdornments(hCommandBar , 0 , 0);
			m_hMenu = CommandBar_GetMenu(hCommandBar , 0);
			GetClientRect(hCommandBar , &rect);
			rect.top += CommandBar_Height(hCommandBar);
			rect.bottom += CommandBar_Height(hCommandBar);
			MoveWindow(hCommandBar , rect.top , rect.left , rect.bottom , rect.right , TRUE);
			#endif
			if(DisplayType == 0) { ico1_hBitmap = LOAD_BITMAP(_T("white_l")) , ico2_hBitmap = LOAD_BITMAP(_T("black_l")); }
			else { ico1_hBitmap = LOAD_BITMAP(_T("white")) ,ico2_hBitmap = LOAD_BITMAP(_T("black")); }
			if (!ico1_hBitmap || !ico2_hBitmap)
			{
				MessageBox(hWnd , _T("Cannot load image file(s)!") , _T("Reversi"), MB_OK | MB_ICONERROR);
				ReleaseDC(m_hWnd, wDC);
				PostQuitMessage(0);
			}

			m_whiteBrush = CreateSolidBrush(RGB(255 , 255 , 255));
			m_blackBrush = CreateSolidBrush(RGB(0 , 0 , 0));
			m_grayBrush = CreateSolidBrush(RGB(0 , 128 , 0));
			m_greenBrush = CreateSolidBrush(RGB(0 , 255 , 0));
			m_lightBlueBrush = CreateSolidBrush(RGB(0xBE , 0xEF , 0xFF));

			m_board = new Board;

			font.lfWidth = font.lfEscapement = font.lfOrientation = 0;
			font.lfWeight = FW_LIGHT;
			font.lfItalic = font.lfUnderline = font.lfStrikeOut = FALSE;
			font.lfCharSet = DEFAULT_CHARSET;
			font.lfOutPrecision = OUT_DEFAULT_PRECIS;
			font.lfClipPrecision = CLIP_CHARACTER_PRECIS;
			font.lfQuality = 5;
			font.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
			font.lfFaceName[0] = _T('\0');

			font.lfHeight = 50;
			m_hFont = CreateFontIndirect(&font);

			wDC = GetDC(m_hWnd);
			board_bufferBMP = CreateCompatibleBitmap(wDC, board_w + 1, board_w + 1);
			board_tDC = CreateCompatibleDC(wDC);
			SelectObject(board_tDC, board_bufferBMP);
			text_bufferBMP = CreateCompatibleBitmap(wDC, 120, 50);
			text_tDC = CreateCompatibleDC(wDC);
			SelectObject(text_tDC, text_bufferBMP);
			RefreshBuffer();
			break;
		case WM_DESTROY:
			delete m_board;
			DeleteObject(m_hFont);
			DeleteObject(m_blackBrush);
			DeleteObject(m_whiteBrush);
			DeleteObject(m_grayBrush);
			DeleteObject(m_lightBlueBrush);
			DeleteObject(m_greenBrush);
			DeleteObject(board_tDC);
			DeleteObject(board_bufferBMP);
			DeleteObject(board_tDC);
			DeleteObject(board_bufferBMP);
			DeleteObject(ico1_hBitmap);
			DeleteObject(ico2_hBitmap);
			ReleaseDC(m_hWnd, wDC);
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			wDC = BeginPaint(m_hWnd , &ps);
			DrawBuffer(wDC);
			EndPaint(m_hWnd, &ps);
			break;
		#ifndef _WIN32_WCE
		case WM_MOUSEMOVE:
		{
			pos = MAKEPOINTS(lParam);
			if ((pos.x >= 25 && pos.x <= 425) && (pos.y >= 25 && pos.y <= 425))
			{
				// in rect
				pos.x -= board_x;
				pos.x /= board_s;
				pos.y -= board_y;
				pos.y /= board_s;
				if (m_board->getBoard()[pos.y][pos.x] == BlockStatus::AVAILABLE) { SetCursor(hand); }
			}
			else { SetCursor(normal); }
			break;
		}
		#endif
		default: return DefWindowProc(hWnd , message , wParam , lParam);
	}
	return 0;
}

int main()
{
	HDC hDC = CreateDC(_T("DISPLAY") , NULL , NULL , NULL);
	const auto deviceCaps = GetDeviceCaps(hDC , HORZRES);
	DeleteDC(hDC);
	if(deviceCaps < 800)
	{
		if(deviceCaps < 480)
		{
			DisplayType = 0;
			board_s = 30;
			text_x = 300;
			text_y = 30;
		}
		else
		{
			DisplayType = 1;
			board_s = 50;
			text_x = 30;
			text_y = 500;
		}
	}
	else
	{
		DisplayType = 2;
		board_s = 50;
		text_x = 500;
		text_y = 30;
	}
	board_x = board_y = 0;
	board_w = board_s * 8;

	WNDCLASS wndClass = {0};
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(0);
	wndClass.hIcon = (HICON)LoadIcon(wndClass.hInstance, TEXT("REVERSI"));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 128, 0)); // 背景は緑
	#ifndef _WIN32_WCE
	wndClass.lpszMenuName = _T("WNDMENU");
	#endif
	wndClass.lpszClassName = _T("Reversi");
	if(!RegisterClass(&wndClass)) { return -1; }

	/*RECT clientRect;
	clientRect.left = 0;
	clientRect.top = 0;
	clientRect.right = 700;
	clientRect.bottom = 480;
	AdjustWindowRectEx (
		&clientRect,
		WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
		TRUE ,
		0
	);*/
	constexpr DWORD style = 
	#ifdef _WIN32_WCE
	WS_VISIBLE
	#else
	WS_OVERLAPPEDWINDOW | WS_VISIBLE
	#endif
	;
	m_hWnd = CreateWindow (
		wndClass.lpszClassName ,
		TEXT("Genkaiya Reversi 3.0 BETA3") ,
		style ,
		CW_USEDEFAULT , CW_USEDEFAULT ,
		CW_USEDEFAULT , CW_USEDEFAULT ,
		NULL ,
		m_hMenu ,
		wndClass.hInstance ,
		NULL
	);
	if (m_hWnd == NULL) { return -1; }

	//ShowWindow(m_hWnd , SW_SHOW);
	//UpdateWindow(m_hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
