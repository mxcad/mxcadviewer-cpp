/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#if defined(MXLINUX)
#include <assert.h>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <list>
#include <memory>
#include <string.h>
#include <GL/glx.h>
#define MX_GLFW_MOUSE_BUTTON_8 7
#define MX_GLFW_MOUSE_BUTTON_LAST      MX_GLFW_MOUSE_BUTTON_8

#include "MxCADInclude.h"
typedef void* HWND;
class MxOpenGLView : public MxCADOpenGlView
{
public:
	MxOpenGLView();
	virtual ~MxOpenGLView();

	bool createWindow(stuCreateMxDrawParam* pParam) override;
	bool destroyWindows() override;
	void glMakeContextCurrent() override;
	void glMakeContextCurrentNull() override;
	void glSwapBuffers() override;
	bool moveWindow(int posx, int posy, int width, int height) override;
	bool glGetMouseButton(int button) override;
	bool invalidate()  override;
	void* glGetCurrentContext() override;
	void glMakeContextCurrent(void* hContextCurrent) override;
public:
	HWND m_hWnd;
	Window m_window;
	Display* m_hDisplay;

	GLXContext   m_hGlContext;

	char  m_mouseButtons[MX_GLFW_MOUSE_BUTTON_8 + 1];

	static MxOpenGLView* m_pCurent;
};

class MxOpenGLDriver
{
public:
	MxOpenGLDriver();
	~MxOpenGLDriver();

	static MxOpenGLDriver* getInstance();
	static void destroyInstance();
private:
	bool chooseVisualInfo();
	static MxOpenGLDriver* s_pInstance;

public:
	Display* m_hDisplay;
	GLXFBConfig m_defFBConfig;
	XVisualInfo* m_defVisualInfo;
};

#endif