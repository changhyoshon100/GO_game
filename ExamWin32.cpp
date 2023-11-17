#include "pch.h"
#include "framework.h"
#include "ExamWin32.h"
#include <vector>
#include <string>
#include <windows.h>
#include <cmath>
#include <iostream>
#include <locale>
#include <cmath>

/*
순서
1. 가로, 세로줄 19 바둑판만들기
2. 화면의 왼쪽에서 왼쪽면까지의 거리(여백)
3. 화면의 위쪽에서 위쪽면까지의 거리(여백)
4. 줄사이의 간격이 26이다. 바둑 돌의 지름 크기도 동일하다.
5. 바둑 돌의 반지름은 줄 간격의 절반과 동일하다
6. 바둑판에 돌이 놓일수 있는 위치를 좀더 편하게 계산할수 있도록
매크로 함수로 만들었다. x와 y에 0-18값을 사용하면 실제 화면상의
좌표로 알 수 있다.
*/
#define X_COUNT 19
#define Y_COUNT 19

#define START_X 10
#define START_Y 10

#define INTERVAL 26
#define HALF_INTERVAL INTERVAL/2
#define XPOS(x) (START_X+(x)*INTERVAL)
#define YPOS(y) (START_Y+(y)*INTERVAL)

/*
바둑판에 돌이 놓여져 있는지 기억하기 위한 변수 배열의 각 항목의 값이0
이면 돌이 놓여져있지 않다는 뜻이고 1이면 검은돌 2이면 흰색 돌이 놓여져 있다는 뜻이다.
현재 어떤 돌을 놓을 것인지를 결정하는 변수(0이면 검, 1이면 흰)
*/

unsigned char g_dol[Y_COUNT][X_COUNT];

void onPaintLine(HWND hWnd) {
    HDC h_dc = GetDC(hWnd);
    HPEN h_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ h_old_pen = SelectObject(h_dc, h_pen);

    for (int x = 0; x < 19; x++) {
        MoveToEx(h_dc, XPOS(x), YPOS(0), NULL);
        LineTo(h_dc, XPOS(x), YPOS(Y_COUNT - 1));
    }
    for (int y = 0; y < 19; y++) {
        MoveToEx(h_dc, XPOS(0), YPOS(y), NULL);
        LineTo(h_dc, XPOS(X_COUNT-1), YPOS(y));
    }
    DeleteObject(h_pen);
}

void onClickBtn(HWND hWnd, LPARAM lParam, bool *g_step) {
    
    HDC h_dc = GetDC(hWnd);
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    if (x <= XPOS(X_COUNT) && x >= START_X && y <= YPOS(Y_COUNT) && y >= START_Y) {
        x = round((x - START_X)/ INTERVAL); 
        y = round((y - START_Y)/ INTERVAL);
        int loc_x = x* INTERVAL + START_X;
        int loc_y = y* INTERVAL + START_Y;
        if (!g_dol[x][y]) {
            HBRUSH h_circle_brush = CreateSolidBrush(RGB(100, 100, 255));
            if (*g_step == true) {
                h_circle_brush = CreateSolidBrush(RGB(0, 0, 0));
                g_dol[x][y] = 1;
            }
            else if(*g_step == false) {
                h_circle_brush = CreateSolidBrush(RGB(255, 255, 255));
                g_dol[x][y] = 2;
            }
            HGDIOBJ h_old_brush = SelectObject(h_dc, h_circle_brush);
            Ellipse(h_dc, loc_x - HALF_INTERVAL,
                loc_y + HALF_INTERVAL,
                loc_x + HALF_INTERVAL,
                loc_y - HALF_INTERVAL);
            SelectObject(h_dc, h_old_brush);
            DeleteObject(h_circle_brush);
            *g_step = !(*g_step);
        }
    }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static bool g_step = false;
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
    }
    else if (uMsg == WM_LBUTTONDOWN) {
        HDC h_dc = GetDC(hWnd);
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        HBRUSH h_circle_brush = CreateSolidBrush(RGB(0, 0, 0));
        HGDIOBJ h_old_brush = SelectObject(h_dc, h_circle_brush);
        
        onClickBtn(hWnd, lParam, &g_step);


    }
    else if (uMsg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC h_dc = BeginPaint(hWnd, &ps);
        HBRUSH h_brush = CreateSolidBrush(RGB(244, 176, 77));
        HGDIOBJ h_old_dc = SelectObject(h_dc, h_brush);
        
        onPaintLine(hWnd);
        DeleteObject(h_brush);
        EndPaint(hWnd, &ps);
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;
    wchar_t my_class_name[] = L"tipssoft";

    HBRUSH h_bk_brush = CreateSolidBrush(RGB(244, 176, 77));

    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    //wc.hbrBackground = (HBRUSH)COLOR_WINDOW;배경색 지정
    wc.hbrBackground = h_bk_brush;//배경색 갈색으로 지정
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = my_class_name;
    wc.lpszMenuName = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wc);

    HWND hWnd = CreateWindow(my_class_name, L"www.tipsoft.com", WS_OVERLAPPEDWINDOW, 100, 90, 400, 350,
        NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(h_bk_brush);

    return msg.wParam;
}