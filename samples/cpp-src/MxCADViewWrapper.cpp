#include "MxCADViewWrapper.h"
#include "../../mx/code/src/mxcadlib/MxCADLib/inc/MxCADExport.h"

struct MxCADViewWrapper::Private
{
	MxCADView m_view;
};

MxCADViewWrapper::MxCADViewWrapper()
	: m_p(new Private)
{
}

MxCADViewWrapper::~MxCADViewWrapper()
{
}

bool MxCADViewWrapper::Create(int width, int height, void* hParentWnd, bool isVisible, int posx, int posy)
{
	return m_p->m_view.Create(width, height, (HWND)hParentWnd, isVisible, posx, posy);
}

bool MxCADViewWrapper::Free()
{
	return m_p->m_view.Free();
}

bool MxCADViewWrapper::ReadFile(const char* pszFileName)
{
	return m_p->m_view.ReadFile(pszFileName);
}

void MxCADViewWrapper::UpdateDisplay(bool isClearOpenGL)
{
    m_p->m_view.UpdateDisplay(isClearOpenGL);
}

bool MxCADViewWrapper::SetMxDrawPosition(int posx, int posy, int width, int height)
{
	return m_p->m_view.SetMxDrawPosition(posx, posy, width, height);
}
