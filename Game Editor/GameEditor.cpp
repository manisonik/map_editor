// GameEditor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GameEditor.h"
#include "CMSKStatusBar.h"
#include "CMSKToolBar.h"
#include "CMSKSplitterWnd.h"
#include "CMSKDialogWnd.h"
#include "CMSKRebar.h"
#include "CMSKTabCtrl.h"
#include "CMSKWindow.h"
#include "CMSKOGLWnd.h"
#include "CMSKTreeView.h"
#include "CMSKPropertyList.h"
#include "CMSKPropertyView.h"

#define MAX_LOADSTRING 100
#define MOUSE_SENSITITY 0.01f;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HGLRC	hRC = NULL;								// Permanent Rendering Context
HDC		hDC = NULL;								// Private GDI Device Context
HWND	hWnd = NULL;							// Holds Our Window Handle
BOOL    keys[256];                              // Array Used For The Keyboard Routine
BOOL    active = TRUE;                          // Window Active Flag Set To TRUE By Default
BOOL    fullscreen = TRUE;                      // Fullscreen Flag Set To Fullscreen Mode By Default
FLOAT windowWidth;
FLOAT windowHeight;
float zoom = 1.0f;
float x_pan = 0.0f;
float y_pan = 0.0f;
bool mouseDown = false;
POINT lastMousePos = { 0, 0 };

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
GLvoid				ResizeGL(GLsizei width, GLsizei height);
GLboolean			InitGL(GLvoid);
GLint				DrawScene(GLvoid);
GLboolean			CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag);
GLvoid				KillGLWindow(GLvoid);
GLvoid				DrawGrid(float size, float count);

CMSKWindow* window;
CMSKStatusBar* statusBar;
CMSKToolBar* toolBar;
CMSKToolBar* toolBar2;
CMSKSplitterWnd* splitWnd;
CMSKSplitterWnd* splitWnd2;
CMSKWindow* child1;
CMSKWindow* child2;
CMSKWindow* child3;
CMSKRebar* rebar;
CMSKTabCtrl* tab;
CMSKOGLWnd* oglWnd;
CMSKTreeView* treeView;
CMSKPropertyList* propertyList;
CMSKPropertyView* propertyView;

INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	// Parts for the status bar
	int parts = { 100 };

	// Top level window
	window = new CMSKWindow(L"Test", 0, 0, 800, 600);
	window->SetBkColor(RGB(240, 240, 240));

	// Create status bar
	statusBar = new CMSKStatusBar(window);
	statusBar->ShowBorder(false);
	statusBar->SetBkColor(RGB(70, 130, 180));
	statusBar->SetParts(&parts);
	statusBar->SetText(L"Ready", 0, SBT_OWNERDRAW | SBT_NOBORDERS, NULL);

	// Create Rebar
	rebar = new CMSKRebar(window);

	// Create toolbar
	toolBar = new CMSKToolBar(L"wd", rebar);

	// Create dialog bar
	child1 = new CMSKDialogWnd(L"Layers", window);
	child1->SetBkColor(RGB(240, 240, 240));

	// Create tab
	tab = new CMSKTabCtrl(child1);

	// Create empty window
	child3 = new CMSKWindow(window);

	// Create vertical split wnd
	splitWnd2 = new CMSKSplitterWnd(window, child1, child3, true);

	// Load icons
	HIMAGELIST imageList = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 3, 4);
	HICON icon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_FOLDERCLOSED), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	ImageList_AddIcon(imageList, icon);
	icon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_FOLDEROPEN), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	ImageList_AddIcon(imageList, icon);
	icon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPDEFAULT), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	ImageList_AddIcon(imageList, icon);

	// Add toolbar and tree view to tab control
	propertyView = new CMSKPropertyView(L"Layer", tab);
	HPVITEM root = propertyView->AddCategory(L"Physics");
	propertyView->AddProperty(PV_BOOLEAN, root);
	propertyView->AddProperty(PV_STRING, root);
	root = propertyView->AddCategory(L"World");
	propertyView->AddProperty(PV_FLOAT, root);
	//treeView->SetImageList(imageList);
	/*HTREEITEM root = propertyList->InsertItem(L"Physics", TVI_ROOT, TVI_LAST, 2, 2);
	propertyList->InsertItem(L"Mass", root, TVI_LAST, 0, 1);
	propertyList->InsertItem(L"Type", root, TVI_LAST, 0, 1);
	propertyList->InsertItem(L"Position", root, TVI_LAST, 0, 1);
	propertyList->InsertItem(L"Bullet", root, TVI_LAST, 0, 1);
	propertyList->InsertItem(L"Active", root, TVI_LAST, 0, 1);
	propertyList->ExpandItem(root);*/
	toolBar2 = new CMSKToolBar(L"Other", tab);

	// Create Opengl window
	oglWnd = new CMSKOGLWnd(window);

	// Create Splitter window
	splitWnd = new CMSKSplitterWnd(window, splitWnd2, oglWnd, false);
	splitWnd->SetBkColor(RGB(240, 240, 240));
	splitWnd->SetBorder(5, 5, 5, 5);

	MSG msg;
	BOOL done = FALSE;
	while (!done) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				done = TRUE;              
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);           
			}
		} else {
			oglWnd->DrawScene();
		}
	}

	return (int)msg.wParam;
}

GLboolean CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag) {

	GLuint		PixelFormat;		// Holds The Results After Searching For A Match
	WNDCLASS	wc;					// Windows Class Structure
	DWORD       dwExStyle;			// Window Extended Style
	DWORD       dwStyle;			// Window Style
	fullscreen = fullscreenflag;	// Set The Global Fullscreen Flag

	RECT WindowRect;                            // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;                  // Set Left Value To 0
	WindowRect.right = (long)width;             // Set Right Value To Requested Width
	WindowRect.top = (long)0;                   // Set Top Value To 0
	WindowRect.bottom = (long)height;           // Set Bottom Value To Requested Height

	hInst = GetModuleHandle(NULL);							// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;			// Redraw On Move, And Own DC For Window
	wc.lpfnWndProc = (WNDPROC)WndProc;						// WndProc Handles Messages
	wc.cbClsExtra = 0;										// No Extra Window Data
	wc.cbWndExtra = 0;										// No Extra Window Data
	wc.hInstance = hInst;									// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);					// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				// Load The Arrow Pointer
	wc.hbrBackground = NULL;								// No Background Required For GL
	wc.lpszMenuName = 0; // MAKEINTRESOURCE(IDC_GAMEEDITOR);									// We Don't Want A Menu
	wc.lpszClassName = L"OpenGL";							// Set The Class Name

	// Attempt To Register The Window Class
	if (!RegisterClass(&wc)) {
		MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;  // Exit And Return FALSE
	}

	if (fullscreen) {  // Attempt Fullscreen Mode?
		DEVMODE dmScreenSettings;										// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));			// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);				// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;							// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;							// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;							// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			// If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.
			if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",
				L"NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {

				// Select Windowed Mode (Fullscreen=FALSE)
				fullscreen = FALSE;
			}
			else {
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, L"Program Will Now Close.", L"ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;                   // Exit And Return FALSE
			}
		}
	}

	if (fullscreen) { // Are We Still In Fullscreen Mode?
		dwExStyle = WS_EX_APPWINDOW;			// Window Extended Style
		dwStyle = WS_POPUP;						// Windows Style
		ShowCursor(FALSE);                      // Hide Mouse Pointer
	}
	else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;     // Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;						// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);     // Adjust Window To True Requested Size

	if (!(hWnd = CreateWindowEx(dwExStyle,              // Extended Style For The Window
		L"OpenGL",										// Class Name
		title,											// Window Title
		WS_CLIPSIBLINGS |								// Required Window Style
		WS_CLIPCHILDREN |								// Required Window Style
		dwStyle,										// Selected Window Style
		0, 0,											// Window Position
		WindowRect.right - WindowRect.left,				// Calculate Adjusted Window Width
		WindowRect.bottom - WindowRect.top,				// Calculate Adjusted Window Height
		NULL,											// No Parent Window
		NULL,											// No Menu
		hInst,											// Instance
		NULL)))											// Don't Pass Anything To WM_CREATE
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	static  PIXELFORMATDESCRIPTOR pfd =                  // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
		1,												// Version Number
		PFD_DRAW_TO_WINDOW |							// Format Must Support Window
		PFD_SUPPORT_OPENGL |							// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,								// Must Support Double Buffering
		PFD_TYPE_RGBA,									// Request An RGBA Format
		bits,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,								// Color Bits Ignored
		0,												// No Alpha Buffer
		0,												// Shift Bit Ignored
		0,												// No Accumulation Buffer
		0, 0, 0, 0,										// Accumulation Bits Ignored
		16,												// 16Bit Z-Buffer (Depth Buffer)
		0,												// No Stencil Buffer
		0,												// No Auxiliary Buffer
		PFD_MAIN_PLANE,									// Main Drawing Layer
		0,												// Reserved
		0, 0, 0											// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))                         // Did We Get A Device Context?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))             // Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))               // Are We Able To Set The Pixel Format?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))                   // Are We Able To Get A Rendering Context?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))                        // Try To Activate The Rendering Context
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);                       // Show The Window
	SetForegroundWindow(hWnd);                      // Slightly Higher Priority
	SetFocus(hWnd);                             // Sets Keyboard Focus To The Window
	ResizeGL(width, height);                       // Set Up Our Perspective GL Screen
}

GLvoid KillGLWindow(GLvoid) {
	if (fullscreen) {
		// If So Switch Back To The Desktop
		ChangeDisplaySettings(NULL, 0);

		// Show Mouse Pointer
		ShowCursor(TRUE);
	}

	// Do We Have A Rendering Context?
	if (hRC) {
		// Are We Able To Release The DC And RC Contexts?
		if (!wglMakeCurrent(NULL, NULL)) {
			MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		// Are We Able To Delete The RC?
		if (!wglDeleteContext(hRC)) {
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		// Set RC To NULL
		hRC = NULL;
	}

	// Are We Able To Release The DC
	if (hDC && !ReleaseDC(hWnd, hDC)) {
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		// Set DC To NULL
		hDC = NULL;
	}

	// Are We Able To Destroy The Window?
	if (hWnd && !DestroyWindow(hWnd)) {
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		// Set hWnd To NULL
		hWnd = NULL;
	}

	// Are We Able To Unregister Class
	if (!UnregisterClass(L"OpenGL", hInst)) {
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		// Set hInstance To NULL
		hInst = NULL;
	}
}

GLint DrawScene(GLvoid) {
	// Clear The Screen And The Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Select The Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset The Projection Matrix
	glLoadIdentity();

	// Calculate The Aspect Ratio Of The Window
	gluOrtho2D(-windowWidth / 2.0f * zoom, 
		windowWidth / 2.0f * zoom, 
		-windowHeight / 2.0f * zoom,
		windowHeight / 2.0f * zoom
	);

	glTranslatef(x_pan, -y_pan, 0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();                // Reset the model-view matrix
	glTranslatef(0.0f, 0.0f, 0.0f);  // Move right and into the screen

	DrawGrid(100, 100);

	// Everything Went OK
	return TRUE;
}

void DrawGrid(float size, float count) {
	float hs = count / 2.0f;

	float w = size * hs;
	float h = size * hs;

	glBegin(GL_LINES);
	for (int i = -hs; i <= hs; i++) {
		if (i % 5 == 0) {
			glColor3f(0.75f, 0.75f, 0.75f);
		} else {
			glColor3f(0.35f, 0.35f, 0.35f);
		}
		glVertex2f(w, i * size);
		glVertex2f(-w, i * size);
	}

	for (int i = -hs; i <= hs; i++) {
		if (i % 5 == 0) {
			glColor3f(0.75f, 0.75f, 0.75f);
		} else {
			glColor3f(0.35f, 0.35f, 0.35f);
		}
		glVertex2f(i * size, h);
		glVertex2f(i * size, -h);
	}
	glEnd();
}

GLboolean InitGL(GLvoid) {
	// Enable smooth shading
	glShadeModel(GL_SMOOTH);

	// Clear to black
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Depth Buffer Setup
	glClearDepth(1.0f);

	// Enables Depth Testing
	glEnable(GL_DEPTH_TEST);

	// The Type Of Depth Test To Do
	glDepthFunc(GL_LEQUAL);

	// Really Nice Perspective Calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return true;
}

GLvoid ResizeGL(GLsizei width, GLsizei height) {
	if (height == 0) {
		height = 1;
	}

	windowWidth = width;
	windowHeight = height;

	glViewport(0, 0, width, height);

	// Select The Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset The Projection Matrix
	glLoadIdentity();

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gluOrtho2D(-windowWidth / 2.0f * zoom, windowWidth / 2.0f * zoom, -windowHeight / 2.0f * zoom, windowHeight / 2.0f * zoom);

	// Select The Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEEDITOR));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GAMEEDITOR);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_MOUSEWHEEL: {
		zoom += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		return 0;
	}
	case WM_LBUTTONDOWN: {
		lastMousePos.x = GET_X_LPARAM(lParam);
		lastMousePos.y = GET_Y_LPARAM(lParam);
		mouseDown = true;
		break;
	}

	case WM_LBUTTONUP: {
		lastMousePos.x = 0;
		lastMousePos.y = 0;
		mouseDown = false;
		break;
	}

	case WM_MOUSEMOVE: {
		if (mouseDown) {
			POINT currentPos = { GET_X_LPARAM(lParam),  GET_Y_LPARAM(lParam) };

			x_pan += (currentPos.x - lastMousePos.x);
			y_pan += (currentPos.y - lastMousePos.y);

			lastMousePos = currentPos;
		}
		break;
	}

	case WM_ACTIVATE:                       // Watch For Window Activate Message
	{
		if (!HIWORD(wParam))                    // Check Minimization State
		{
			active = TRUE;                    // Program Is Active
		}
		else
		{
			active = FALSE;                   // Program Is No Longer Active
		}

		return 0;                       // Return To The Message Loop
	}
	case WM_SYSCOMMAND:                     // Intercept System Commands
	{
		switch (wParam)                     // Check System Calls
		{
		case SC_SCREENSAVE:             // Screensaver Trying To Start?
		case SC_MONITORPOWER:               // Monitor Trying To Enter Powersave?
			return 0;                   // Prevent From Happening
		}
		break;                          // Exit
	}
	case WM_CLOSE:                          // Did We Receive A Close Message?
	{
		PostQuitMessage(0);                 // Send A Quit Message
		return 0;                       // Jump Back
	}
	case WM_KEYDOWN:                        // Is A Key Being Held Down?
	{
		keys[wParam] = TRUE;                    // If So, Mark It As TRUE
		return 0;                       // Jump Back
	}
	case WM_KEYUP:                          // Has A Key Been Released?
	{
		keys[wParam] = FALSE;                   // If So, Mark It As FALSE
		return 0;                       // Jump Back
	}
	case WM_SIZE:                           // Resize The OpenGL Window
	{
		ResizeGL(LOWORD(lParam), HIWORD(lParam));       // LoWord=Width, HiWord=Height
		return 0;                       // Jump Back
	}
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
			PostQuitMessage(0);                 // Send A Quit Message
			break;
		}
	}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, message, wParam, lParam);
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
