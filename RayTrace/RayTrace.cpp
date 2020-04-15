// RayTrace.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
///////////////////////////////////
//
//
//
//
//
void RenderByRayTrace(HDC& dc,int width , int height);

#include "Sphere.h"
#include "sinetexture.h"
#include "ImageTexture.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RAYTRACE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_RAYTRACE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_RAYTRACE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_RAYTRACE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			GetClientRect(hWnd, &rt);
			
			//RenderByRayTrace(hdc,rt.right-rt.left,rt.bottom-rt.top); 
			RenderByRayTrace(hdc,256,256); 
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}






////////////////////////////////////////
//
//
//
//
//
//
//
//
//
//
//
void RenderByRayTrace(HDC& dc,int width , int height)
{

  int *imagedata = new int[width*height];
  
  Texture *texture;

#if 1
  texture = new SineTexture (2.5, 13.3, 4);

#else

texture = new ImageTexture ("butterfly.bmp");

#endif







  Light* light = new DirectionalLight (.6, .4, .6);
  Obj *obj = new Sphere (width, width * 1 / 2, texture, light);

  int sup = 2;
  double supInv = 1.0 / sup;
  for (int j = 0; j < height; ++ j) {
     for (int i = 0; i < width; ++ i) {
        pRGB *pixel = new pRGB (0.0, 0.0, 0.0);
        for (int k = 0; k < sup; ++ k) {
          for (int l = 0; l < sup; ++ l) {
            Vec *ray = new Vec ((i * 2. - width) / 6. + k * supInv,
                               (j * 2. - height) / 6. + l * supInv, width / 4);
            pRGB *rgb = obj->getIntersection (*ray);
            pixel->add (*rgb);
			delete ray;
			
          }
        }
        pixel->scale (supInv * supInv);
        imagedata[i + width * j] = pixel->toRGB ();
	    delete pixel;
		
		
      }


  }


 BITMAPINFO  bfh;
            LPVOID BitmapBuf = 0; 			
			memset(&bfh,0,sizeof(BITMAPINFO));

			bfh.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bfh.bmiHeader.biWidth = width;
			bfh.bmiHeader.biHeight = height;
			bfh.bmiHeader.biPlanes = 1;
			bfh.bmiHeader.biBitCount = 32;
			bfh.bmiHeader.biCompression =  BI_RGB;
            bfh.bmiHeader.biClrUsed =  256*256*256; 
            bfh.bmiHeader.biClrImportant = 0;  
			HBITMAP hbm = CreateDIBSection(dc,&bfh,
				DIB_RGB_COLORS , (void **)&BitmapBuf,
				0,0);
			memcpy(BitmapBuf,(void *)imagedata , width*height*4 );
            GdiFlush();
		
			HDC hdc = CreateCompatibleDC(0);

			HBITMAP oldbitmap = 
			(HBITMAP)SelectObject(hdc,hbm);

			BitBlt(dc,0,0,abs(width),abs(height),hdc,0,0,SRCCOPY);
			SelectObject(hdc,oldbitmap);
			DeleteDC(hdc);
			DeleteObject(oldbitmap);


            return ;
















}





