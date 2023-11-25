/*
 * Dependencies:
 *  gdi32
 *  (kernel32)
 *  user32
 *  (comctl32)
 *
 */

#include <tchar.h>
#include <windows.h>
#include <math.h>
#include <iostream>     
#include<fstream>
#include "Header.h"
#include <string>


#define KEY_SHIFTED     0x8000
#define KEY_TOGGLED     0x0001

int* Input() {

    /*
      1 - При помощи отображения файлов на память (CreateFileMapping/MapViewOfFile)
      2 - При помощи файловых переменных (fopen / fread / fwrite / fclose).
      3 - При помощи потоков ввода-вывода (fstream).
      4 - При помощи файловых функций WinAPI/NativeAPI*/

    int N = 3; 
    int save = 100;

    LPWSTR cmdLine = GetCommandLine(); 
    LPWSTR* argv; 
    int argc;

    argv = CommandLineToArgvW(cmdLine, &argc); 

    if (argc == 2)
    {
        N = _wtoi(argv[1]);
        if (N < 1 || N > 99){
            N = 3;
        }
    }
    else if (argc > 2)
    {
        N = _wtoi(argv[1]);
        if (N < 1 || N > 99) {
            N = 3;
        }

        save = _wtoi(argv[2]);
        if (save < 0 || save > 3) {
            save = 3;
        }
    }

    LocalFree(argv);

    int arr[2];
    arr[0] = N;
    arr[1] = save;

    return arr;
}

const TCHAR szWinClass[] = _T("Win32SampleApp");
const TCHAR szWinName[] = _T("Win32SampleWindow");
HWND hwnd;               /* This is the handle for our window */
HBRUSH hBrush;              // current brush  
HPEN hBrushBounds;            
RECT rec = { 0 };
int width = 0;
int height = 0;
int cell = 3;
int cells[100][100];
int saveType;
COLORREF boundscolor;
COLORREF background;
int typeofsave;

void GetRGBValues(COLORREF color, int* r, int* g, int* b)
{
    *r = GetRValue(color);
    *g = GetGValue(color);
    *b = GetBValue(color);
}

int rgb(int b, int delta) {
    if (b + delta <= 0)
        return 0;
    else if (b + delta >= 255)
        return 255;
    else
        return (b + delta) % 255;
}

/* Runs Notepad */
void RunNotepad(void)
{
    STARTUPINFO sInfo;
    PROCESS_INFORMATION pInfo;

    ZeroMemory(&sInfo, sizeof(STARTUPINFO));

    puts("Starting Notepad...");
    CreateProcess(_T("C:\\Windows\\Notepad.exe"),
        NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

void Save() {
    SaveFile(std::to_string(cell) + "\r\n" +
                    std::to_string(width) + "\r\n" + std::to_string(height) + "\r\n" +
                    std::to_string(GetRValue(background)) + "\r\n" +
                    std::to_string(GetGValue(background)) + "\r\n" +
                    std::to_string(GetBValue(background)) + "\r\n" +
                    std::to_string(GetRValue(boundscolor)) + "\r\n" +
                    std::to_string(GetGValue(boundscolor)) + "\r\n" +
                    std::to_string(GetBValue(boundscolor)) + "\r\n", typeofsave);
    int k;
}

void Read() {
    int* saved = ReadFile(typeofsave);
    if (saved != NULL){
        cell = saved[0];
        width = saved[1];
        height = saved[2];
        background = RGB(saved[3], saved[4], saved[5]);
        boundscolor = RGB(saved[6], saved[7], saved[8]);
    }
    else {
        cell = 3;
        width = 320;
        height = 240;
        boundscolor = RGB(255, 0, 0);
        background = RGB(0, 110, 255);
    }
}

void WriteToBuf() {
    std::string str;
    for (int i = 0; i < cell; i++)
        for (int j = 0; j < cell; j++)
            str += std::to_string(cells[i][j]);

    const char* cstr = str.c_str();
    RewiteBuf(str);
}

void ReadFromBuf(std::string answ) {
    for (int i = 0; i < answ.length(); i++)
        //std::cout<< (int)(answ[i]) - '0' << " " << i / cell << " " << i % cell << std::endl;
        cells[i / cell][i % cell] = (int)((answ[i]) - '0');
    InvalidateRect(hwnd, NULL, TRUE);
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY: {
            CloseBuf();
            Save();
            PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
            break;
        }

        case WM_SIZE: {
            GetClientRect(hwnd, &rec);

            width = (rec.right - rec.left);
            height = (rec.bottom - rec.top);

            InvalidateRect(hwnd, NULL, TRUE);

            break;
        }

        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam) / 10;

            int r, g, b;
            GetRGBValues(boundscolor, &r, &g, &b);

            boundscolor = RGB(rgb(r, delta), rgb(g, delta), rgb(b, delta));

            // Redraw the window
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            hBrushBounds = CreatePen(PS_DOT, 1, boundscolor);
            SelectObject(hdc, hBrushBounds);
            SetBkColor(hdc, RGB(0, 0, 0));

            int sizex = int(width * 1.0 / cell);
            int sizey = int(height * 1.0 / cell);
            for (int i = 0; i < cell + 1; i++) {
                MoveToEx(hdc, 0, sizey * i, NULL);
                LineTo(hdc, width, sizey * i);
            }
            for (int i = 0; i < cell + 1; i++) {
                MoveToEx(hdc, sizex * i, 0, NULL);
                LineTo(hdc, sizex * i, height);
            }
            DeleteObject(hBrushBounds);

            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            for (int i = 0; i < cell; i++)
            {
                for (int j = 0; j < cell;j++)
                    if (cells[i][j]==1) {
                        MoveToEx(hdc, i*sizex, j*sizey, NULL);
                        LineTo(hdc, (i+1) * sizex, (j + 1)* sizey);
                        MoveToEx(hdc, (i+1) * sizex, j * sizey, NULL);
                        LineTo(hdc, i * sizex, (j + 1) * sizey);
                    }
                    else if (cells[i][j] == 2) {
                        Ellipse(hdc, i * sizex, j * sizey, (i + 1) * sizex, (j + 1) * sizey);
                    } 
            }

            EndPaint(hwnd, &ps);      
            break;
        }

        case WM_LBUTTONDOWN: {
            int mousex = lParam & 0xFFFF;
            int mousey = (lParam >> 16) & 0xFFFF;

            cells[int(mousex / int(width * 1.0 / cell))][int(mousey / int(height * 1.0 / cell))] = 2;
            InvalidateRect(hwnd, NULL, TRUE);

            WriteToBuf();
            break;
        }

        case WM_RBUTTONDOWN: {
            int mousex = lParam & 0xFFFF;
            int mousey = (lParam >> 16) & 0xFFFF;

            cells[int(mousex / int(width * 1.0 / cell))][int(mousey / int(height * 1.0 / cell))] = 1;
            InvalidateRect(hwnd, NULL, TRUE);
            WriteToBuf();
            break;
        }

        case WM_KEYUP:
            switch (wParam) {
                case VK_CONTROL:
                    if (GetKeyState('Q') & (0x8000 > 0)) {
                        PostQuitMessage(0);
                    }
                    break;
                case VK_ESCAPE: {
                    PostQuitMessage(0);             
                    break;
                }
                case VK_SHIFT: {
                    if (GetKeyState('C') & (0x8000 > 0)) {
                        RunNotepad();
                    }
                    break;
                }
                case VK_RETURN: {
                    background = RGB(rand() % 100, rand() % 100, rand() % 100);
                    hBrush = CreateSolidBrush(background);
                    HBRUSH oldBrush = (HBRUSH)SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
                    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
                    DeleteObject(oldBrush);
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);
                    break;
                }
                
                break;
            }
        default:
        {
            if (message == msgReg) {
                std::string str(pBufExchange);
                ReadFromBuf(str);
            } else
                return DefWindowProc(hwnd, message, wParam, lParam);
            break;
        }
    }
    return 0;
    /* for messages that we don't deal with */
}


int main(int argc, char** argv)
{
    typeofsave = 3;
    Read();
    cell = Input()[0];
    for (int i = 0; i < cell; i++)
    {
        for (int j = 0; j < cell;j++)
            cells[i][j] =0 ;
    }
    msgReg = RegisterWindowMessageA("qweertyufdsadsfgdsfg6547657654kjhlkjh430kqnl8fd");
    Init(msgReg);

    printf("size: %d\n", cell);
    printf("save type: %d\n", typeofsave);

    BOOL bMessageOk;
    MSG message;            /* Here message to the application are saved */
    WNDCLASS wincl = { 0 };         /* Data structure for the windowclass */

    /* Harcode show command num when use non-winapi entrypoint */
    int nCmdShow = SW_SHOW;
    /* Get handle */
    HINSTANCE hThisInstance = GetModuleHandle(NULL);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szWinClass;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by Windows */

    /* Use custom brush to paint the background of the window */
    hBrush = CreateSolidBrush(background);
    wincl.hbrBackground = hBrush;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClass(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindow(
        szWinClass,          /* Classname */
        _T("TicTacToe"),       /* Title Text */
        WS_OVERLAPPEDWINDOW, /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        width,                 /* The programs width */
        height,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    std::string str222(pBufExchange);
    ReadFromBuf(str222);
    ReadFromBuf(str222);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while ((bMessageOk = GetMessage(&message, NULL, 0, 0)) != 0)
    {
        /* Yep, fuck logic: BOOL mb not only 1 or 0.
         * See msdn at https://msdn.microsoft.com/en-us/library/windows/desktop/ms644936(v=vs.85).aspx
         */
        if (bMessageOk == -1)
        {
            puts("Suddenly, GetMessage failed! You can call GetLastError() to see what happend");
            break;
        }
        /* Translate virtual-key message into character message */
        TranslateMessage(&message);
        /* Send message to WindowProcedure */
        DispatchMessage(&message);
    }

    /* Cleanup stuff */
    DestroyWindow(hwnd);
    UnregisterClass(szWinClass, hThisInstance);
    DeleteObject(hBrush);

    //RunNotepad();

    return 0;
}