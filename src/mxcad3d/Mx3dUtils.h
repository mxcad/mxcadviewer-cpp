/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QString>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Aspect_VKey.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Window.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <Poly_Triangulation.hxx>
#include <variant>
#include "Mx3dBndBoxCoords.h"
#include "Mx3dMeasureShape.h"
#include <QList>
#include <QPair>
#include <TDF_Label.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <XCAFDoc_Material.hxx>

namespace Mx3d {

	

	

	
	bool hasClipPlane(const Handle(V3d_View)& view, const Handle(Graphic3d_ClipPlane)& plane);
	bool isReversedStandardDir(const gp_Dir& n);
	std::pair<double, double> getRange(BndBoxCoords bbc, gp_Dir planeNormal);


	// BREP 
	bool brepCircle(const TopoDS_Shape& shape, MeasureCircle& mc);
	bool brepLength(const TopoDS_Shape& shape, MeasureLength& ml);
	bool brepArea(const TopoDS_Shape& shape, MeasureArea& ma);
	bool brepBoundingBox(const TopoDS_Shape& shape, MeasureBoundingBox& mbb);

	bool brepMinDistance(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2, MeasureDistance& md);
	bool brepCenterDistance(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2, MeasureDistance& md);
	bool brepAngle(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2, MeasureAngle& ma);

	bool brepCircleFromGeometricEdge(const TopoDS_Edge& edge, MeasureCircle& mc);
	bool brepCircleFromPolygonEdge(const TopoDS_Edge& edge, MeasureCircle& mc);
	double triangleArea(const gp_XYZ& p1, const gp_XYZ& p2, const gp_XYZ& p3);
	double triangulationArea(const Handle(Poly_Triangulation)& triangulation);
	bool computeShapeCenter(const TopoDS_Shape& shape, gp_Pnt& pt);


	// PROPERTY
	const TCollection_ExtendedString& labelAttrStdName(const TDF_Label& label);
	QString getShapeTypeStr(TopAbs_ShapeEnum shapeEnum);
	QString getXdeShapeTypeStr(const TDF_Label& label);
	QString getLayersStr(const TDF_Label& label);
	QString getLocationStr(const TDF_Label& label);
	bool isSetColor(const TDF_Label& label);
	QString getColorStr(const TDF_Label& label);
	Handle(XCAFDoc_Material) getShapeMaterial(const TDF_Label& lbl);
	QString getMaterialNameStr(const TDF_Label& label);
	QString getProductNameStr(const TDF_Label& label);

	using Properties = QList<QPair<QString, QString>>;
	Properties getLabelProperties(const TDF_Label& label);
	
} // namespace Mx3d


namespace OcctQtTools
{
    Aspect_VKeyMouse qtMouseButtons2VKeys(Qt::MouseButtons theButtons);

    Aspect_VKeyFlags qtMouseModifiers2VKeys(Qt::KeyboardModifiers theModifiers);

    Aspect_VKey qtKey2VKey(int theKey);



	QString extStrToQtStr(const TCollection_ExtendedString& str);

} // namespace OcctQtTools


namespace OcctGlTools
{ 
	Handle(OpenGl_Context) GetGlContext(const Handle(V3d_View)& theView);
} // namespace OcctGlTools


