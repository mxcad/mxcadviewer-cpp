#pragma once

#include <memory>

class MxCADViewWrapper
{
public:
	MxCADViewWrapper();
	~MxCADViewWrapper();

	bool Create(int width, int height, void* hParentWnd, bool isVisible = true, int posx = 0, int posy = 0);

	bool Free();


	bool   ReadFile(const char* pszFileName);


	void   UpdateDisplay(bool isClearOpenGL = true);

	bool   SetMxDrawPosition(int posx, int posy, int width, int height);

private:
	struct Private;
    std::unique_ptr<Private> m_p;
};

