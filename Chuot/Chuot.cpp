
#include "framework.h"
#include "Chuot.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHUOT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHUOT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHUOT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_PEN);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHUOT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Chuột
/*
    * Kiểm tra có chuột hay không: int fMouse = GetSystemMetrics(SM_MOUSEPRESENT)
    * Xác định số nút trên chuột: int cButtons = GetSystemMetrics(SM_CMOUSEBUTTONS) ;
    * Một số con trỏ chuột có sẵn:
        - IDC_ARROW: mũi tên
        - IDC_CROSS: Hình dấu cộng
        - IDC_WAIT: Hình vòng tròn xoay
    * Chuột có 3 nút
        - LBUTTON: nút trái
        - MBUTTON: nút giữa
        - RBUTTON: nút phải
    * Windows định nghĩa 21 thông điệp với chuột:
        - Thông điệp trong miền thao tác: (10 thông điệp)
           + WM_MOUSEMOVE : di chuyển trong các cửa sổ của miền thao tác
             chuột trái: WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK (tương tự với chuột phải, giữa)
           + lParam chứa vị trí chuột so với gics trên bên trái của miền thao tác
                x = LOWORD(lParam)
                y = HIWORD(lParam)
           + wParam cho biết trạng thái của các nút chuột và các phím Shift, Ctrl
        - Thông điệp ngoài miền thao tác: (11 thông điệp)
*/


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT point[1000];
    static int n=0;
    static HDC hdc;
    static HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    switch (message)
    {
    case WM_LBUTTONDBLCLK: // DBL click chuột trái -> xóa miền thao tác
        // xóa dữ liệu trong array point
        n = 0;  // point là mảng tĩnh nên không xóa được
        // Xóa miền thao tác đã vẽ
        InvalidateRect (hWnd, NULL, TRUE);
        break;
    case WM_LBUTTONDOWN:    // click chuột trái -> vẽ 1 điểm trên miền thao tác và lưu tọa độ
        // Lấy tọa độ hiện tại -> point[n]
        point[n].x = LOWORD(lParam);
        point[n].y = HIWORD(lParam);
        // Vẽ điểm
        hdc = GetDC(hWnd);  // Lấy handle để vẽ
        SetPixel(hdc, point[n].x, point[n].y, RGB(255, 0, 0));
        ReleaseDC(hWnd, hdc);
        // Vẽ xong
        n++;
        break;
    case WM_RBUTTONDOWN:    // click chuột phải: nối từng đôi một các điểm lại
        if (n >= 2) // phải có ít nhất 2 điểm mới nối được
        {
            for (int i = 0; i < n-1; i++)
            {
                for (int j = i + 1; j < n; j++)
                {
                    hdc = GetDC(hWnd);
                    // Lấy bút pen vẽ cho đẹp :))
                    SelectObject(hdc, pen);
                    MoveToEx(hdc, point[i].x, point[i].y, NULL);
                    LineTo(hdc, point[j].x, point[j].y);
                    ReleaseDC(hWnd, hdc);
                }
            }
        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
