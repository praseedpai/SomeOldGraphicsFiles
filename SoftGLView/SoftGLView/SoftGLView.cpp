// SoftGLView.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "softgl.hpp"
#include "trimodel.hpp"

void Render( const HDC& hdc );


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
	LoadString(hInstance, IDC_SOFTGLVIEW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_SOFTGLVIEW);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_SOFTGLVIEW);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_SOFTGLVIEW;
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
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			Render(hdc );
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

#if 1

void Render( const HDC& hdc )
{

  SoftGL sgl(600,400);
  sgl.Clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);

  sgl.MatrixMode(PROJECTION);
  sgl.Ortho(-1,1,-1,1,-1,1);

 // sgl.Color3f(1,0,0);
 // sgl.Begin(POLYGON);
 //   sgl.Vertex3f(-2,-2,0);
 //   sgl.Vertex3f(2,-2,0);
 //   sgl.Vertex3f(2,2,0);
 //   sgl.Vertex3f(-2,2,0);
 // sgl.End();
 sgl.ShadeModel(FLAT);
  sgl.Begin(TRIANGLES);
 // sgl.Color3f(0,1,0);
   // sgl.Vertex3f(0,1.2f,0.5f);
   // sgl.Vertex3f(-1,-1.2f,0.5f);
   // sgl.Vertex3f(1,-5,0.5f);
  sgl.Color3f(0,0,1);
    sgl.Vertex3f(0.5f,0.5f,0.6f);
	  sgl.Color3f(1,0,0);
    sgl.Vertex3f(-0.6f,0.1f,0.6f);
	sgl.Color3f(0,1,0);
    sgl.Vertex3f(0.7f,-0.2f,0.6f);
  sgl.End();
/*
  sgl.Color3f(0,0,1);
  sgl.Begin(TRIANGLE_FAN);
    sgl.Vertex3f(0,0,0.8f);
    sgl.Vertex3f(2,0,0.8f);
    sgl.Vertex3f(2,1,0.8f);
    sgl.Vertex3f(2,2,0.8f);
  sgl.End();
*/
//  sgl.DrawToPPM("output.ppm");

  sgl.RenderDevice(hdc);
  

}



#else
	
void DrawTriModel(TriModel *TM,SoftGL& sgl)
{
  sgl.Begin(TRIANGLES);

  for (int i=0; i<TM->NumTris; i++)
  {
    sgl.Color3ubv(TM->Tris[i].Color);
    sgl.Normal3fv(TM->Tris[i].N);
    sgl.Vertex3fv(TM->Tris[i].A);
    sgl.Vertex3fv(TM->Tris[i].B);
    sgl.Vertex3fv(TM->Tris[i].C);
  }

  sgl.End();
}

void Render( const HDC& hdc )
{
	
SoftGL sgl(256,256);
TriModel *TM;



//  TM = new TriModel("teapot.tri",255,128,128);
  TM = new TriModel("tie.tri");
//  TM = new TriModel("onetri.tri");
  TM->Normalize();

  sgl.ShadeModel(FLAT);

  sgl.Clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);

  sgl.MatrixMode(PROJECTION);
//  sgl.Ortho(-1,1,-1,1,-1,1);
  sgl.Perspective(30,1,0.1f,10);
  sgl.MatrixMode(MODELVIEW);
  sgl.LookAt(2,2,2, 0,0,0, 0,1,0);

  DrawTriModel(TM,sgl);

  //  sgl.DrawToPPM("output.ppm");

  sgl.RenderDevice(hdc);
  



}
#endif


/*

void DrawTriModel(TriModel *TM,SoftGL& sgl)
{
  sgl.Begin(TRIANGLES);

  for (int i=0; i<TM->NumTris; i++)
  {
    sgl.Color3ubv(TM->Tris[i].Color);
    sgl.Normal3fv(TM->Tris[i].N);
    sgl.Vertex3fv(TM->Tris[i].A);
    sgl.Vertex3fv(TM->Tris[i].B);
    sgl.Vertex3fv(TM->Tris[i].C);
  }

  sgl.End();
}

void Render( const HDC& hdc )
{
	SoftGL sgl(256,256);
TriModel *TM;

  TM = new TriModel("teapot.tri",255,128,128);
 // TM = new TriModel("tie.tri");
  //TM = new TriModel("onetri.tri");
  TM->Normalize();

  sgl.ShadeModel(FLAT);

  sgl.Clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);

  sgl.MatrixMode(PROJECTION);
// sgl.Ortho(-1,1,-1,1,-1,1);
  sgl.Perspective(10,1,0.1f,10);
  sgl.MatrixMode(MODELVIEW);
  sgl.LookAt(5,2,5, 0,0,0, 0,1,0);

  DrawTriModel(TM,sgl);

//  sgl.DrawToPPM("output.ppm");

  sgl.RenderDevice(hdc);

}

#endif
*/