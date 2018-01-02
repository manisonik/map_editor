#include "stdafx.h"
#include "CMSKOGLWnd.h"

CMSKOGLWnd::CMSKOGLWnd(CMSKWindow* parent) {
	GLuint		PixelFormat;		// Holds The Results After Searching For A Match
	DWORD       dwExStyle;			// Window Extended Style
	DWORD       dwStyle;			// Window Style
	m_bFullScreen = false;			// Set The Global Fullscreen Flag

	// Store instance
	hInst = parent->GetInstanceHandle();

	// Register Opengl class
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;			// Redraw On Move, And Own DC For Window
	wc.lpfnWndProc = (WNDPROC) RouterProc;						// WndProc Handles Messages
	wc.cbClsExtra = 0;										// No Extra Window Data
	wc.cbWndExtra = 0;										// No Extra Window Data
	wc.hInstance = hInst;									// Set The Instance
	wc.hIcon = 0;											// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				// Load The Arrow Pointer
	wc.hbrBackground = NULL;								// No Background Required For GL
	wc.lpszMenuName = 0;									// We Don't Want A Menu
	wc.lpszClassName = L"MSKOGLClass";						// Set The Class Name

	// Register class;
	RegisterClass(&wc);

	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = L"";
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = L"MSKOGLClass";
	cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)4;
	PreCreateWindow(cs);

	// Bind window messages
	Bind(WM_SIZE, (MSGFN)&CMSKOGLWnd::OnSize);
	Bind(WM_DESTROY, (MSGFN)&CMSKOGLWnd::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKOGLWnd::OnCreate);

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	static  PIXELFORMATDESCRIPTOR pfd =                  // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
		1,												// Version Number
		PFD_DRAW_TO_WINDOW |							// Format Must Support Window
		PFD_SUPPORT_OPENGL |							// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,								// Must Support Double Buffering
		PFD_TYPE_RGBA,									// Request An RGBA Format
		16,												// Select Our Color Depth
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

	if (!(m_hDC = GetDC(m_hWnd)))                         // Did We Get A Device Context?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
	}

	if (!(PixelFormat = ChoosePixelFormat(m_hDC, &pfd)))             // Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
	}

	if (!SetPixelFormat(m_hDC, PixelFormat, &pfd))               // Are We Able To Set The Pixel Format?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
	}

	if (!(m_hRC = wglCreateContext(m_hDC)))                   // Are We Able To Get A Rendering Context?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
	}

	if (!wglMakeCurrent(m_hDC, m_hRC))                        // Try To Activate The Rendering Context
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
	}

	Initialize();
}

CMSKOGLWnd::~CMSKOGLWnd()
{
}

int CMSKOGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	return 0;
}

void CMSKOGLWnd::OnSize(UINT nType, int cx, int cy) {
	ResizeGL(cx, cy);
	DrawScene();
}

void CMSKOGLWnd::OnDestroy() {
	KillGLWindow();
}

GLvoid CMSKOGLWnd::KillGLWindow(GLvoid) {
	if (m_bFullScreen) {
		// If So Switch Back To The Desktop
		ChangeDisplaySettings(NULL, 0);

		// Show Mouse Pointer
		ShowCursor(TRUE);
	}

	// Do We Have A Rendering Context?
	if (m_hRC) {
		// Are We Able To Release The DC And RC Contexts?
		if (!wglMakeCurrent(NULL, NULL)) {
			MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		// Are We Able To Delete The RC?
		if (!wglDeleteContext(m_hRC)) {
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		// Set RC To NULL
		m_hRC = NULL;
	}

	// Are We Able To Release The DC
	if (m_hDC && !ReleaseDC(m_hWnd, m_hDC)) {
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		// Set DC To NULL
		m_hDC = NULL;
	}

	// Are We Able To Destroy The Window?
	if (m_hWnd && !DestroyWindow(m_hWnd)) {
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		// Set hWnd To NULL
		m_hWnd = NULL;
	}

	// Are We Able To Unregister Class
	/*if (!UnregisterClass(L"MSKOGLClass", hInst)) {
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		// Set hInstance To NULL
		hInst = NULL;
	}*/
}

GLint CMSKOGLWnd::DrawScene(GLvoid) {
	// Clear The Screen And The Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Select The Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset The Projection Matrix
	glLoadIdentity();

	// Calculate The Aspect Ratio Of The Window
	gluOrtho2D(-m_fWindowWidth / 2.0f * m_fZoom,
		m_fWindowWidth / 2.0f * m_fZoom,
		-m_fWindowHeight / 2.0f * m_fZoom,
		m_fWindowHeight / 2.0f * m_fZoom
	);

	glTranslatef(m_fPanX, -m_fPanY, 0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();                // Reset the model-view matrix
	glTranslatef(0.0f, 0.0f, 0.0f);  // Move right and into the screen

	DrawGrid(100, 100);

	SwapBuffers(m_hDC);

	// Everything Went OK
	return TRUE;
}

void CMSKOGLWnd::DrawGrid(float size, float count) {
	float hs = count / 2.0f;

	float w = size * hs;
	float h = size * hs;

	glBegin(GL_LINES);
	for (int i = -hs; i <= hs; i++) {
		if (i % 5 == 0) {
			glColor3f(0.75f, 0.75f, 0.75f);
		}
		else {
			glColor3f(0.35f, 0.35f, 0.35f);
		}
		glVertex2f(w, i * size);
		glVertex2f(-w, i * size);
	}

	for (int i = -hs; i <= hs; i++) {
		if (i % 5 == 0) {
			glColor3f(0.75f, 0.75f, 0.75f);
		}
		else {
			glColor3f(0.35f, 0.35f, 0.35f);
		}
		glVertex2f(i * size, h);
		glVertex2f(i * size, -h);
	}
	glEnd();
}

GLboolean CMSKOGLWnd::Initialize(GLvoid) {
	// Enable smooth shading
	glShadeModel(GL_SMOOTH);

	// Clear to black
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

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

GLvoid CMSKOGLWnd::ResizeGL(GLsizei width, GLsizei height) {
	if (height == 0) {
		height = 1;
	}

	m_fWindowWidth = width;
	m_fWindowHeight = height;

	glViewport(0, 0, width, height);

	// Select The Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset The Projection Matrix
	glLoadIdentity();

	// Calculate The Aspect Ratio Of The Window
	gluOrtho2D(-m_fWindowWidth / 2.0f * m_fZoom, m_fWindowWidth / 2.0f * m_fZoom, -m_fWindowHeight / 2.0f * m_fZoom, m_fWindowHeight / 2.0f * m_fZoom);

	// Select The Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
}

/*if (m_bFullScreen) {  // Attempt Fullscreen Mode?
DEVMODE dmScreenSettings;										// Device Mode
memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));			// Makes Sure Memory's Cleared
dmScreenSettings.dmSize = sizeof(dmScreenSettings);				// Size Of The Devmode Structure
dmScreenSettings.dmPelsWidth = width;							// Selected Screen Width
dmScreenSettings.dmPelsHeight = height;							// Selected Screen Height
dmScreenSettings.dmBitsPerPel = 16;								// Selected Bits Per Pixel
dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
// If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.
if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",
L"NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {

// Select Windowed Mode (Fullscreen=FALSE)
m_bFullScreen = FALSE;
}
else {
// Pop Up A Message Box Letting User Know The Program Is Closing.
MessageBox(NULL, L"Program Will Now Close.", L"ERROR", MB_OK | MB_ICONSTOP);
}
}
}

if (m_bFullScreen) { // Are We Still In Fullscreen Mode?
dwExStyle = WS_EX_APPWINDOW;			// Window Extended Style
dwStyle = WS_POPUP;						// Windows Style
ShowCursor(FALSE);                      // Hide Mouse Pointer
}
else {
dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;     // Window Extended Style
dwStyle = WS_OVERLAPPEDWINDOW;						// Windows Style
}*/

//AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);     // Adjust Window To True Requested Size