/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxOpenGLView.h"
#if defined(MXLINUX)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
MxOpenGLView* MxOpenGLView::m_pCurent = 0;

MxOpenGLView::MxOpenGLView() {
	m_hWnd = 0;
	m_hGlContext = 0;

	m_hDisplay = 0;
	
	for (int i = 0; i <= MX_GLFW_MOUSE_BUTTON_LAST; i++)
	{
		m_mouseButtons[i] = 0;
	}

}
MxOpenGLView::~MxOpenGLView() {

	if (m_hDisplay != NULL)
	{
		glXMakeCurrent(m_hDisplay, None, NULL);
		glXWaitGL();
		glXDestroyContext(m_hDisplay, m_hGlContext);
	}
}

//! Search for RGBA double-buffered visual with stencil buffer.
static int TheDoubleBuffVisual[] =
{
  GLX_RGBA,
  GLX_DEPTH_SIZE, 16,
  GLX_STENCIL_SIZE, 1,
  GLX_DOUBLEBUFFER,
  None
};

//! Search for RGBA double-buffered visual with stencil buffer.
static int TheDoubleBuffFBConfig[] =
{
  GLX_X_RENDERABLE,  True,
  GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
  GLX_RENDER_TYPE,   GLX_RGBA_BIT,
  GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
  GLX_DEPTH_SIZE,    16,
  GLX_STENCIL_SIZE,  1,
  GLX_DOUBLEBUFFER,  True,
  None
};

bool OpenGl_Context_CheckExtension(const char* theExtString,
	const char* theExtName)
{
	if (theExtString == NULL)
	{
		return false;
	}

	char* aPtrIter = (char*)theExtString;
	const char*  aPtrEnd = aPtrIter + strlen(theExtString);
	const size_t anExtNameLen = strlen(theExtName);
	while (aPtrIter < aPtrEnd)
	{
		const size_t n = strcspn(aPtrIter, " ");
		if ((n == anExtNameLen) && (strncmp(aPtrIter, theExtName, anExtNameLen) == 0))
		{
			return true;
		}
		aPtrIter += (n + 1);
	}
	return false;
}

//! Dummy XError handler which just skips errors
static int xErrorDummyHandler(Display*     /*theDisplay*/,XErrorEvent* /*theErrorEvent*/)
{
	return 0;
}

bool MxOpenGLView::createWindow(stuCreateMxDrawParam* pParam) {

	Window     aWindow = (Window)m_hWnd;
	m_window = aWindow;

	MxOpenGLDriver* pGLDriver = MxOpenGLDriver::getInstance();
	m_hDisplay = pGLDriver->m_hDisplay;
	if (m_hDisplay == 0) {
		return false;
	}

	GLXContext aGContext = 0;
	GLXContext aSlaveCtx = 0;

	Display* aDisp = m_hDisplay;
	XWindowAttributes aWinAttribs;
	XGetWindowAttributes(aDisp, aWindow, &aWinAttribs);

	XVisualInfo aVisInfo;
	aVisInfo.visualid = aWinAttribs.visual->visualid;
	aVisInfo.screen = DefaultScreen(aDisp);
	int aNbItems;
	std::unique_ptr<XVisualInfo, int(*)(void*)> aVis(XGetVisualInfo(aDisp, VisualIDMask | VisualScreenMask, &aVisInfo, &aNbItems), &XFree);

	int isGl = 0;
	if (aVis.get() == NULL)
	{
		return false;
	}
	else if (glXGetConfig(aDisp, aVis.get(), GLX_USE_GL, &isGl) != 0 || !isGl)
	{
		return false;
	}

	GLXFBConfig anFBConfig = 0;	
	if (pGLDriver->m_defVisualInfo) {
		anFBConfig = pGLDriver->m_defFBConfig;
	}

	const char* aGlxExts = glXQueryExtensionsString(aDisp, aVisInfo.screen);
	bool isCoreProfile = false;
	bool myOwnGContext = true;
	bool contextCompatible = false;
	bool contextDebug = false;

	if (myOwnGContext
		&& anFBConfig != NULL
		&& OpenGl_Context_CheckExtension(aGlxExts, "GLX_ARB_create_context_profile"))
	{
		// Replace default XError handler to ignore errors.
		// Warning - this is global for all threads!
		typedef int(*xerrorhandler_t)(Display*, XErrorEvent*);
		xerrorhandler_t anOldHandler = XSetErrorHandler(xErrorDummyHandler);

		typedef GLXContext(*glXCreateContextAttribsARB_t)(Display* dpy, GLXFBConfig config,
			GLXContext share_context, Bool direct,
			const int* attrib_list);

		glXCreateContextAttribsARB_t aCreateCtxProc = (glXCreateContextAttribsARB_t)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
		if (!contextCompatible)
		{
			int aCoreCtxAttribs[] =
			{
			  GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			  GLX_CONTEXT_MINOR_VERSION_ARB, 2,
			  GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
			  GLX_CONTEXT_FLAGS_ARB,         contextDebug ? GLX_CONTEXT_DEBUG_BIT_ARB : 0,
			  0, 0
			};

			for (int aLowVer4 = 6; aLowVer4 >= 0 && aGContext == NULL; --aLowVer4)
			{
				aCoreCtxAttribs[1] = 4;
				aCoreCtxAttribs[3] = aLowVer4;
				aGContext = aCreateCtxProc(aDisp, anFBConfig, aSlaveCtx, True, aCoreCtxAttribs);
			}
			for (int aLowVer3 = 3; aLowVer3 >= 2 && aGContext == NULL; --aLowVer3)
			{
				aCoreCtxAttribs[1] = 3;
				aCoreCtxAttribs[3] = aLowVer3;
				aGContext = aCreateCtxProc(aDisp, anFBConfig, aSlaveCtx, True, aCoreCtxAttribs);
			}
			isCoreProfile = aGContext != NULL;
		}

		if (aGContext == NULL)
		{
			int aCtxAttribs[] =
			{
			  GLX_CONTEXT_FLAGS_ARB, contextDebug ? GLX_CONTEXT_DEBUG_BIT_ARB : 0,
			  0, 0
			};
			isCoreProfile = false;
			aGContext = aCreateCtxProc(aDisp, anFBConfig, aSlaveCtx, True, aCtxAttribs);

			if (aGContext != NULL && !contextCompatible)
			{
				//TCollection_ExtendedString aMsg("OpenGl_Window::CreateWindow: core profile creation failed.");
				//myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW, aMsg);
			}
		}
		XSetErrorHandler(anOldHandler);
	}

	if (myOwnGContext
		&& aGContext == NULL)
	{
		aGContext = glXCreateContext(aDisp, aVis.get(), aSlaveCtx, GL_TRUE);
		if (aGContext == NULL)
		{
			return false;
		}
	}

	m_hGlContext = aGContext;
	this->glMakeContextCurrent();

	if (!onInitOpenGLGlad((void*)glXGetProcAddress)) {
		return false;
	}
	return true;
}

bool MxOpenGLView::destroyWindows() {
	return true;
}

void MxOpenGLView::glMakeContextCurrent() {
	glXMakeCurrent((Display*)m_hDisplay, (GLXDrawable)m_window, (GLXContext)m_hGlContext);
	m_pCurent = this;

}

void MxOpenGLView::glMakeContextCurrentNull() {
	glXMakeCurrent(m_hDisplay, None, NULL);
	m_pCurent = 0;
}

void MxOpenGLView::glSwapBuffers() {
	glXSwapBuffers(m_hDisplay, m_window);

}

bool MxOpenGLView::moveWindow(int posx, int posy, int width, int height) {
	this->glMakeContextCurrent();
	glViewport(0, 0, width, height);
	return true;
}

bool MxOpenGLView::glGetMouseButton(int button) {
	if (button > MX_GLFW_MOUSE_BUTTON_LAST) return false;
	return (m_mouseButtons[button] == 1);
}

bool MxOpenGLView::invalidate() {
	//::InvalidateRect(m_hWnd, NULL, TRUE);
	return false;
}

void* MxOpenGLView::glGetCurrentContext() {
	return (void*)m_pCurent;
}

void MxOpenGLView::glMakeContextCurrent(void* hContextCurrent) {
	m_pCurent = (MxOpenGLView*)hContextCurrent;
	if (m_pCurent) {
		m_pCurent->glMakeContextCurrent();
	}
	else {
		glXMakeCurrent(m_hDisplay, None, NULL);
	}
}


MxOpenGLDriver* MxOpenGLDriver::s_pInstance = 0;

MxOpenGLDriver::MxOpenGLDriver() {
	m_defVisualInfo = 0;
	m_defFBConfig = 0;

	char *result = getenv("DISPLAY");
	m_hDisplay = XOpenDisplay(result);
	if (m_hDisplay != 0) {
		chooseVisualInfo();
	}
}



MxOpenGLDriver::~MxOpenGLDriver() {
	if (m_defVisualInfo != NULL)
	{
		XFree(m_defVisualInfo);
	}

	if (m_hDisplay != NULL)
	{
		XCloseDisplay((Display*)m_hDisplay);
	}
}

MxOpenGLDriver* MxOpenGLDriver::getInstance() {
	if (s_pInstance == 0) {
		s_pInstance = new MxOpenGLDriver();
	}
	return s_pInstance;
}

void MxOpenGLDriver::destroyInstance() {
	delete s_pInstance;
	s_pInstance = 0;
}


// =======================================================================
// function : chooseVisualInfo
// purpose  :
// =======================================================================
bool MxOpenGLDriver::chooseVisualInfo()
{
	Display* aDisp = (Display*)m_hDisplay;

	XVisualInfo* aVisInfo = NULL;
	GLXFBConfig anFBConfig = NULL;

	int aScreen = DefaultScreen(aDisp);
	int aDummy = 0;
	if (!XQueryExtension(aDisp, "GLX", &aDummy, &aDummy, &aDummy)
		|| !glXQueryExtension(aDisp, &aDummy, &aDummy))
	{
		return false;
	}

	// FBConfigs were added in GLX version 1.3
	int aGlxMajor = 0, aGlxMinor = 0;
	const bool hasFBCfg = glXQueryVersion(aDisp, &aGlxMajor, &aGlxMinor)
		&& ((aGlxMajor == 1 && aGlxMinor >= 3) || (aGlxMajor > 1));
	if (hasFBCfg)
	{
		int aFBCount = 0;
		GLXFBConfig* aFBCfgList = NULL;
		if (hasFBCfg)
		{
			aFBCfgList = glXChooseFBConfig(aDisp, aScreen, TheDoubleBuffFBConfig, &aFBCount);
		}
		if (aFBCfgList != NULL
			&& aFBCount >= 1)
		{
			anFBConfig = aFBCfgList[0];
			aVisInfo = glXGetVisualFromFBConfig(aDisp, anFBConfig);
		}
		XFree(aFBCfgList);
	}

	if (aVisInfo == NULL)
	{
		aVisInfo = glXChooseVisual(aDisp, aScreen, TheDoubleBuffVisual);
	}

	if (aVisInfo == NULL) {
		return false;
	}
	if (anFBConfig == NULL) {
		return false;
	}

	m_defFBConfig = anFBConfig;
	m_defVisualInfo = aVisInfo;
	return true;
}

#endif