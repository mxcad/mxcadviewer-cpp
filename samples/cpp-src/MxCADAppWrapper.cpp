#include "MxCADAppWrapper.h"
#include "../../mx/code/src/mxcadlib/MxCADLib/inc/MxCADExport.h"

struct  MxCADAppWrapper::Private
{
	MxCADApp m_app;
};

MxCADAppWrapper::MxCADAppWrapper()
	: m_p(new Private)
{
}

MxCADAppWrapper::~MxCADAppWrapper()
{
}

bool MxCADAppWrapper::Free()
{
	return m_p->m_app.Free();
}
