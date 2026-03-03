#include "MxCADObjectWrapper.h"
#include "../../mx/code/src/mxcadlib/MxCADLib/inc/MxCADExport.h"

struct MxCADObjectWrapper::Private
{
	MxCADObject m_obj;
};

MxCADObjectWrapper::MxCADObjectWrapper()
	: m_p(new Private)
{
}

MxCADObjectWrapper::~MxCADObjectWrapper()
{
}

bool MxCADObjectWrapper::Create()
{
	return m_p->m_obj.Create();
}

int MxCADObjectWrapper::ReadFile(const char* fileName)
{
	return m_p->m_obj.ReadFile(fileName);
}

int MxCADObjectWrapper::WriteFile(const char* fileName)
{
	return m_p->m_obj.WriteFile(fileName);
}

int MxCADObjectWrapper::DrawLine(double x1, double y1, double x2, double y2)
{
	auto* pLine = new McDbLine(McGePoint3d(x1, y1, 0), McGePoint3d(x2, y2, 0));
	return (int)MrxDbgUtils::addToCurrentSpaceAndClose(pLine);
}

int MxCADObjectWrapper::DrawCircle(double x, double y, double r)
{
	auto *pCircle = new McDbCircle();
    pCircle->setCenter(McGePoint3d(x, y, 0));
    pCircle->setRadius(r);
    return (int)MrxDbgUtils::addToCurrentSpaceAndClose(pCircle);
}
