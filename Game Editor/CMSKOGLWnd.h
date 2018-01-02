#pragma once
#include "CMSKWindow.h"

class CMSKOGLWnd :
	public CMSKWindow
{
public:
	float m_fWindowWidth;
	float m_fWindowHeight;
	HGLRC	m_hRC = NULL;								
	HDC		m_hDC = NULL;								
	BOOL    m_bKeys[256];                             
	BOOL    m_bActive = TRUE;                          
	BOOL    m_bFullScreen = FALSE;             
	float m_fZoom = 1.0f;
	float m_fPanX = 0.0f;
	float m_fPanY = 0.0f;
	bool m_bMouseDown = false;
	POINT m_ptLastMousePos = { 0, 0 };

public:
	CMSKOGLWnd(CMSKWindow* parent);
	~CMSKOGLWnd();

	GLboolean isActive();
	GLboolean isKeyPressed(int key);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
	void OnDestroy();

	GLvoid ResizeGL(GLsizei width, GLsizei height);
	GLboolean Initialize(GLvoid);
	GLvoid DrawGrid(float size, float count);
	GLint DrawScene(GLvoid);
	GLvoid KillGLWindow(GLvoid);
};

