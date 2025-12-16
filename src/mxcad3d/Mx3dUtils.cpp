/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx3dUtils.h"
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Adaptor3d_Curve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <PrsDim_AngleDimension.hxx>
#include <optional>
#include <GC_MakeCircle.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <TDataStd_TreeNode.hxx>
#include <XCAFDoc.hxx>
#include <QCoreApplication>

namespace Mx3d {



	
    bool hasClipPlane(const Handle(V3d_View)& view, const Handle(Graphic3d_ClipPlane)& plane)
    {
        const Handle(Graphic3d_SequenceOfHClipPlane)& seqClipPlane = view->ClipPlanes();
        if (seqClipPlane.IsNull() || seqClipPlane->Size() == 0)
            return false;

        for (Graphic3d_SequenceOfHClipPlane::Iterator it(*seqClipPlane); it.More(); it.Next()) {
            const Handle(Graphic3d_ClipPlane)& candidate = it.Value();
            if (candidate.get() == plane.get())
                return true;
        }

        return false;
    }

    bool isReversedStandardDir(const gp_Dir& n)
    {
        for (int i = 0; i < 3; ++i) {
            const double c = n.XYZ().GetData()[i];
            if (c < 0 && (std::abs(c) - 1) < Precision::Confusion())
                return true;
        }

        return false;
    }

    std::pair<double, double> getRange(BndBoxCoords bbc, gp_Dir planeNormal)
    {
        const gp_Vec n(isReversedStandardDir(planeNormal) ? planeNormal.Reversed() : planeNormal);
        bool isMaxValid = false;
        bool isMinValid = false;
        double rmax = std::numeric_limits<double>::lowest();
        double rmin = std::numeric_limits<double>::max();
        for (const gp_Pnt& bndPoint : bbc.vertices()) {
            const gp_Vec vec(bndPoint.XYZ());
            const double dot = n.Dot(vec);
            rmax = isMaxValid ? std::max(rmax, dot) : dot;
            rmin = isMinValid ? std::min(rmin, dot) : dot;
            isMaxValid = true;
            isMinValid = true;
        }

        if (isMaxValid && isMinValid)
        {
            double r = abs(rmax - rmin);
            r *= 0.1;
            return { rmin - r, rmax + r };
        }

        return {};
    }

    bool brepCircle(const TopoDS_Shape& shape, MeasureCircle& mc)
    {
        if (shape.IsNull() || shape.ShapeType() != TopAbs_EDGE)
        {
            return false;
        }
        const TopoDS_Edge& edge = TopoDS::Edge(shape);
        if (BRep_Tool::IsGeometric(edge))
            return brepCircleFromGeometricEdge(edge, mc);
        else
            return brepCircleFromPolygonEdge(edge, mc);
    }

    bool brepLength(const TopoDS_Shape& shape, MeasureLength& ml)
    {
        if (shape.IsNull() || shape.ShapeType() != TopAbs_EDGE)
        {
            return false;
        }
        auto d0 = [](const Adaptor3d_Curve& curve, double u)->gp_Pnt {
            gp_Pnt pnt;
            curve.D0(u, pnt);
            return pnt;
            };
        const TopoDS_Edge& edge = TopoDS::Edge(shape);
        if (BRep_Tool::IsGeometric(edge)) {
            const BRepAdaptor_Curve curve(edge);
            const double len = GCPnts_AbscissaPoint::Length(curve, 1e-6);
            ml.value = len * 1.0;
            const GCPnts_QuasiUniformAbscissa pnts(curve, 3);
            if (pnts.IsDone() && pnts.NbPoints() == 3) {
                ml.middlePnt = d0(curve, pnts.Parameter(2));
            }
            else {
                const double midParam = (curve.FirstParameter() + curve.LastParameter()) / 2.;
                ml.middlePnt = d0(curve, midParam);
            }
        }
        else {
            TopLoc_Location loc;
            const Handle(Poly_Polygon3D)& polyline = BRep_Tool::Polygon3D(edge, loc);
            if (polyline.IsNull())
            {
                return false;
            }
            double len = 0.;

            for (int i = 2; i <= polyline->NbNodes(); ++i) {
                const gp_Pnt& pnt1 = polyline->Nodes().Value(i - 1);
                const gp_Pnt& pnt2 = polyline->Nodes().Value(i);
                len += pnt1.Distance(pnt2);
            }

            ml.value = len * 1.0;

            double accumLen = 0.;
            for (int i = 2; i <= polyline->NbNodes() && accumLen < (len / 2.); ++i) {
                const gp_Pnt& pnt1 = polyline->Nodes().Value(i - 1);
                const gp_Pnt& pnt2 = polyline->Nodes().Value(i);
                accumLen += pnt1.Distance(pnt2);
                if (accumLen > (len / 2.)) {
                    const gp_Pnt pntLoc1 = pnt1.Transformed(loc);
                    const gp_Pnt pntLoc2 = pnt2.Transformed(loc);
                    ml.middlePnt = pntLoc1.Translated(gp_Vec{ pntLoc1, pntLoc2 } / 2.);
                }
            }
        }
        return true;
    }

    bool brepArea(const TopoDS_Shape& shape, MeasureArea& ma)
    {
        MeasureArea areaResult;
        if (shape.IsNull() || shape.ShapeType() != TopAbs_FACE)
        {
            return false;
        }
        const TopoDS_Face& face = TopoDS::Face(shape);

        if (BRep_Tool::IsGeometric(face)) {
            GProp_GProps gprops;
            BRepGProp::SurfaceProperties(face, gprops);
            const double area = gprops.Mass();
            ma.value = area * 1.0;

            const BRepAdaptor_Surface surface(face);
            ma.middlePnt = surface.Value(
                (surface.FirstUParameter() + surface.LastUParameter()) / 2.,
                (surface.FirstVParameter() + surface.LastVParameter()) / 2.
            );
        }
        else {
            TopLoc_Location loc;
            const Handle(Poly_Triangulation)& triangulation = BRep_Tool::Triangulation(face, loc);
            if (triangulation.IsNull())
            {
                return false;
            }
            ma.value = triangulationArea(triangulation) * 1.0;

            for (int i = 1; i <= triangulation->NbNodes(); ++i) {
                const gp_Pnt node = triangulation->Node(i).Transformed(loc);
                ma.middlePnt.Translate(node.XYZ());
            }

            ma.middlePnt.ChangeCoord().Divide(triangulation->NbNodes());
        }

        return true;
    }

    bool brepBoundingBox(const TopoDS_Shape& shape, MeasureBoundingBox& mbb)
    {
        Bnd_Box bnd;
        BRepBndLib::AddOptimal(shape, bnd);
        if (bnd.IsVoid())
        {
            return false;
        }
        mbb.cornerMin = bnd.CornerMin();
        mbb.cornerMax = bnd.CornerMax();
        mbb.xLength = std::abs(mbb.cornerMax.X() - mbb.cornerMin.X()) * 1.0;
        mbb.yLength = std::abs(mbb.cornerMax.Y() - mbb.cornerMin.Y()) * 1.0;
        mbb.zLength = std::abs(mbb.cornerMax.Z() - mbb.cornerMin.Z()) * 1.0;
        mbb.volume = mbb.xLength * mbb.yLength * mbb.zLength;
        return true;
    }

    bool brepMinDistance(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2, MeasureDistance& md)
    {
        if (shape1.IsNull() || shape2.IsNull())
        {
            return false;
        }


        BRepExtrema_DistShapeShape dist;
        try {
            dist.LoadS1(shape1);
            dist.LoadS2(shape2);
            dist.Perform();
        }
        catch (...) {
            return false;
        }

        if (!dist.IsDone())
        {
            return false;
        }

        md.pnt1 = dist.PointOnShape1(1);
        md.pnt2 = dist.PointOnShape2(1);
        md.value = dist.Value() * 1.0;
        md.type = MeasureDistance::Type::Mininmum;
        return true;
    }

    bool brepCenterDistance(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2, MeasureDistance& md)
    {
        if (shape1.IsNull() || shape2.IsNull())
        {
            return false;
        }

        gp_Pnt centerOfMass1, centerOfMass2;
        if (!computeShapeCenter(shape1, centerOfMass1) || !computeShapeCenter(shape2, centerOfMass2))
        {
            return false;
        }

        md.pnt1 = centerOfMass1;
        md.pnt2 = centerOfMass2;
        md.value = centerOfMass1.Distance(centerOfMass2) * 1.0;
        md.type = MeasureDistance::Type::CenterToCenter;
        return true;
    }

    bool brepAngle(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2, MeasureAngle& ma)
    {
        if (shape1.IsNull() || shape2.IsNull())
        {
            return false;
        }

        TopoDS_Edge edge1 = TopoDS::Edge(shape1);
        TopoDS_Edge edge2 = TopoDS::Edge(shape2);

        const BRepAdaptor_Curve curve1(edge1);
        const BRepAdaptor_Curve curve2(edge2);



        auto fnGetLineDirection = [](const Adaptor3d_Curve& curve, gp_Dir& dir) -> bool {
            if (curve.GetType() != GeomAbs_Line)
            {
                return false;
            }
            dir = curve.Line().Direction();
            return true;
            };

        gp_Dir dirLine1, dirLine2;

        if (!fnGetLineDirection(curve1, dirLine1) || !fnGetLineDirection(curve2, dirLine2))
        {
            return false;
        }

        if (dirLine1.IsParallel(dirLine2, Precision::Angular()))
        {
            return false;
        }

        if (GCPnts_AbscissaPoint::Length(curve1, 1e-6) > GCPnts_AbscissaPoint::Length(curve2, 1e-6))
            std::swap(edge1, edge2);

        BRepExtrema_DistShapeShape distEdges(edge1, edge2);
        if (!distEdges.IsDone())
        {
            return false;
        }

        const double minDist = distEdges.Value();
        if (minDist > Precision::Confusion()) {
            gp_Trsf trsf;
            trsf.SetTranslation(distEdges.PointOnShape2(1), distEdges.PointOnShape1(1));
            edge2 = TopoDS::Edge(BRepBuilderAPI_Transform(edge2, trsf, true));
        }

        PrsDim_AngleDimension dimAngle(edge1, edge2);
        ma.pnt1 = dimAngle.FirstPoint();
        ma.pnt2 = dimAngle.SecondPoint();
        ma.pntCenter = dimAngle.CenterPoint();
        const gp_Vec vec1(ma.pntCenter, ma.pnt1);
        const gp_Vec vec2(ma.pntCenter, ma.pnt2);
        ma.value = vec1.Angle(vec2) * 1.0;

        return true;
    }

    bool brepCircleFromGeometricEdge(const TopoDS_Edge& edge, MeasureCircle& mc)
    {
        const BRepAdaptor_Curve curve(edge);
        std::optional<gp_Circ> circle;

        auto d0 = [](const Adaptor3d_Curve& curve, double u)->gp_Pnt {
            gp_Pnt pnt;
            curve.D0(u, pnt);
            return pnt;
            };

        if (curve.GetType() == GeomAbs_Circle) {
            circle = curve.Circle();
        }
        else if (curve.GetType() == GeomAbs_Ellipse) {
            const gp_Elips ellipse = curve.Ellipse();
            if (std::abs(ellipse.MinorRadius() - ellipse.MajorRadius()) < Precision::Confusion())
                circle = gp_Circ{ ellipse.Position(), ellipse.MinorRadius() };
        }
        else {
            {
                const GCPnts_QuasiUniformAbscissa pnts(curve, 4);
                if (!pnts.IsDone() || pnts.NbPoints() < 3)
                {
                    return false;
                }

                const GC_MakeCircle makeCirc(
                    d0(curve, pnts.Parameter(1)),
                    d0(curve, pnts.Parameter(2)),
                    d0(curve, pnts.Parameter(3))
                );

                if (!makeCirc.IsDone())
                {
                    return false;
                }
                circle = makeCirc.Value()->Circ();
            }

            {
                const GCPnts_QuasiUniformAbscissa pnts(curve, 64);
                if (!pnts.IsDone())
                {
                    return false;
                }
                for (int i = 1; i <= pnts.NbPoints(); ++i) {
                    const gp_Pnt pntSample = d0(curve, pnts.Parameter(i));
                    const double dist = pntSample.Distance(circle->Location());
                    if (std::abs(dist - circle->Radius()) > 1e-4)
                    {
                        return false;
                    }
                }
            }
        }

        if (!circle)
        {
            return false;
        }
        mc.pntAnchor = d0(curve, curve.FirstParameter());
        mc.isArc = !curve.IsClosed();
        mc.value = circle.value();
        return true;
    }

    bool brepCircleFromPolygonEdge(const TopoDS_Edge& edge, MeasureCircle& mc)
    {
        return false;
    }

    double triangleArea(const gp_XYZ& p1, const gp_XYZ& p2, const gp_XYZ& p3)
    {
        const double ax = p2.X() - p1.X();
        const double ay = p2.Y() - p1.Y();
        const double az = p2.Z() - p1.Z();
        const double bx = p3.X() - p1.X();
        const double by = p3.Y() - p1.Y();
        const double bz = p3.Z() - p1.Z();
        const double cx = ay * bz - az * by;
        const double cy = az * bx - ax * bz;
        const double cz = ax * by - ay * bx;
        return 0.5 * std::sqrt(cx * cx + cy * cy + cz * cz);
    }

    double triangulationArea(const Handle(Poly_Triangulation)& triangulation)
    {
        if (!triangulation)
            return 0;

        double area = 0;

        for (const Poly_Triangle& tri : triangulation->InternalTriangles()) {
            int v1, v2, v3;
            tri.Get(v1, v2, v3);
            area += triangleArea(
                triangulation->Node(v1).Coord(),
                triangulation->Node(v2).Coord(),
                triangulation->Node(v3).Coord()
            );
        }

        return area;
    }

    bool computeShapeCenter(const TopoDS_Shape& shape, gp_Pnt& pt)
    {
        const TopAbs_ShapeEnum shapeType = shape.ShapeType();

        if (shapeType == TopAbs_VERTEX) {
            pt = BRep_Tool::Pnt(TopoDS::Vertex(shape));
            return true;
        }


        GProp_GProps shapeProps;
        if (shapeType == TopAbs_FACE) {
            BRepGProp::SurfaceProperties(shape, shapeProps);
        }
        else if (shapeType == TopAbs_WIRE) {
            BRepGProp::LinearProperties(shape, shapeProps);
        }
        else if (shapeType == TopAbs_EDGE) {
            try {

                Mx3d::MeasureCircle circle;
                bool ok = brepCircle(shape, circle);
                if (ok)
                {
                    pt = circle.value.Location();
                    return true;
                }

            }
            catch (...) {
                BRepGProp::LinearProperties(shape, shapeProps);
            }
        }
        else {
            return false;
        }

        if (shapeProps.Mass() < Precision::Confusion())
        {
            return false;
        }
        pt = shapeProps.CentreOfMass();
        return true;
    }

    const TCollection_ExtendedString& labelAttrStdName(const TDF_Label& label)
    {
        Handle_TDataStd_Name attrName;
        if (label.FindAttribute(TDataStd_Name::GetID(), attrName)) {
            return attrName->Get();
        }
        else {
            static const TCollection_ExtendedString nullStr = {};
            return nullStr;
        }
    }

    QString getShapeTypeStr(TopAbs_ShapeEnum shapeEnum) 
    {
        QString ret;
        switch (shapeEnum)
        {
        case TopAbs_COMPOUND:	ret = "COMPOUND";	break;
        case TopAbs_COMPSOLID:	ret = "COMPSOLID";	break;
        case TopAbs_SOLID:		ret = "SOLID";		break;
        case TopAbs_SHELL:		ret = "SHELL";		break;
        case TopAbs_FACE:		ret = "FACE";		break;
        case TopAbs_WIRE:		ret = "WIRE";		break;
        case TopAbs_EDGE:		ret = "EDGE";		break;
        case TopAbs_VERTEX:		ret = "VERTEX";		break;
        case TopAbs_SHAPE:		ret = "SHAPE";		break;
        }
        return ret;
    }

    QString getXdeShapeTypeStr(const TDF_Label& label) {
        QStringList shapeKind;
        if (XCAFDoc_ShapeTool::IsAssembly(label))
        {
            shapeKind.append("Assembly");
        }
        if (XCAFDoc_ShapeTool::IsReference(label))
        {
            shapeKind.append("Reference");
        }
        if (XCAFDoc_ShapeTool::IsComponent(label))
        {
            shapeKind.append("Component");
        }
        if (XCAFDoc_ShapeTool::IsCompound(label))
        {
            shapeKind.append("Compound");
        }
        if (XCAFDoc_ShapeTool::IsSimpleShape(label))
        {
            shapeKind.append("Simple");
        }
        if (XCAFDoc_ShapeTool::IsSubShape(label))
        {
            shapeKind.append("Sub");
        }
        return shapeKind.join("+");
    }


    QString getLayersStr(const TDF_Label& label) {

        Handle(XCAFDoc_LayerTool) lt = XCAFDoc_DocumentTool::LayerTool(label.Root());
        TDF_LabelSequence seqLayerLabel;
        lt->GetLayers(label, seqLayerLabel);

        if (XCAFDoc_ShapeTool::IsReference(label))
        {
            TDF_Label referredLabel;
            XCAFDoc_ShapeTool::GetReferredShape(label, referredLabel);
            TDF_LabelSequence seqLayerLabelProduct;
            lt->GetLayers(referredLabel, seqLayerLabelProduct);
            seqLayerLabel.Append(seqLayerLabelProduct);
        }

        QStringList listLayerName;
        for (const TDF_Label& layerLabel : seqLayerLabel)
        {
            TCollection_ExtendedString layerName;
            lt->GetLayer(layerLabel, layerName);
            listLayerName.append(OcctQtTools::extStrToQtStr(layerName));
        }
        if (!seqLayerLabel.IsEmpty())
        {
            return listLayerName.join(", ");
        }
        else {
            return QString();
        }

    }

    QString getLocationStr(const TDF_Label& label) {
        const TopLoc_Location loc = XCAFDoc_ShapeTool::GetLocation(label);
        gp_Trsf trsf = loc.Transformation();
        gp_XYZ axisRotation;
        double angleRotation;
        trsf.GetRotation(axisRotation, angleRotation);
        QString value = QString("[(%1, %2, %3);%4;(%5, %6, %7)]").arg(axisRotation.X()).arg(axisRotation.Y()).arg(axisRotation.Z()).arg(angleRotation).arg(gp_Pnt(trsf.TranslationPart()).X()).arg(gp_Pnt(trsf.TranslationPart()).Y()).arg(gp_Pnt(trsf.TranslationPart()).Z());
        return value;
    }

    bool isSetColor(const TDF_Label& label) {
        Handle(XCAFDoc_ColorTool) ct = XCAFDoc_DocumentTool::ColorTool(label.Root());
        return ct->IsSet(label, XCAFDoc_ColorGen) || ct->IsSet(label, XCAFDoc_ColorSurf) || ct->IsSet(label, XCAFDoc_ColorCurv);
    }

    QString getColorStr(const TDF_Label& label) {
        Handle(XCAFDoc_ColorTool) ct = XCAFDoc_DocumentTool::ColorTool(label.Root());
        Quantity_Color color;

        if (ct->GetColor(label, XCAFDoc_ColorGen, color))
        {
        }
        if (ct->GetColor(label, XCAFDoc_ColorSurf, color))
        {
        }
        if (ct->GetColor(label, XCAFDoc_ColorCurv, color))
        {
        }
        return QString("(%1, %2, %3)").arg(color.Red()).arg(color.Green()).arg(color.Blue());
    }

    Handle(XCAFDoc_Material) getShapeMaterial(const TDF_Label& lbl)
    {
        Handle(TDataStd_TreeNode) node;
        if (!lbl.FindAttribute(XCAFDoc::MaterialRefGUID(), node) || !node->HasFather())
            return {};

        const TDF_Label labelMaterial = node->Father()->Label();
        Handle(XCAFDoc_Material) attr;
        labelMaterial.FindAttribute(XCAFDoc_Material::GetID(), attr);
        return attr;
    }

    QString getMaterialNameStr(const TDF_Label& label) {
        TDF_Label referredLabel;
        if (XCAFDoc_ShapeTool::IsReference(label))
        {
            XCAFDoc_ShapeTool::GetReferredShape(label, referredLabel);
        }
        else
        {
            referredLabel = label;
        }
        const Handle(XCAFDoc_Material) material = getShapeMaterial(referredLabel);
        if (material)
        {
            return QString(material->GetName()->ToCString());
        }

        return QString();
    }

    QString getProductNameStr(const TDF_Label& label) {

        if (XCAFDoc_ShapeTool::IsReference(label))
        {
            TDF_Label referredLabel;
            XCAFDoc_ShapeTool::GetReferredShape(label, referredLabel);
            return OcctQtTools::extStrToQtStr(labelAttrStdName(referredLabel));
        }
        else
        {
            return QString();
        }
    }

    Properties getLabelProperties(const TDF_Label& label)
    {
        QList<QPair<QString, QString>> properties;

        QString labelName = OcctQtTools::extStrToQtStr(labelAttrStdName(label));
        properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "Name"), labelName));

        if (!XCAFDoc_ShapeTool::GetShape(label).IsNull())
        {
            QString labelShapeType = getShapeTypeStr(XCAFDoc_ShapeTool::GetShape(label).ShapeType());
            properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "Shape Type"), labelShapeType));
        }
        QString labelXdeType = getXdeShapeTypeStr(label);
        properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "XDE Shape Type"), labelXdeType));

        QString labelLayers = getLayersStr(label);
        properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "Layer"), labelLayers));

        if (XCAFDoc_ShapeTool::IsReference(label))
        {
            QString labelLocation = getLocationStr(label);
            properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "Location"), labelLocation));
        }

        if (isSetColor(label))
        {
            QString labelColor = getColorStr(label);
            properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "Color"), labelColor));

        }
        else if (XCAFDoc_ShapeTool::IsReference(label)) {

            TDF_Label referredLabel;
            XCAFDoc_ShapeTool::GetReferredShape(label, referredLabel);
            if (isSetColor(referredLabel))
            {
                QString labelColor = getColorStr(referredLabel);
                properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "Product Color"), labelColor));
            }
        }
        QString labelMaterial = getMaterialNameStr(label);
        properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "Material Name"), labelMaterial));

        if (XCAFDoc_ShapeTool::IsReference(label))
        {
            QString labelProduct = getProductNameStr(label);
            properties.append(QPair<QString, QString>(QCoreApplication::translate("Mx3dUtils", "Product Name"), labelProduct));
        }

        return properties;
    }

} // namespace Mx3d




namespace OcctQtTools
{

	Aspect_VKeyMouse OcctQtTools::qtMouseButtons2VKeys(Qt::MouseButtons theButtons)
	{
		Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
		if ((theButtons & Qt::LeftButton) != 0)
			aButtons |= Aspect_VKeyMouse_LeftButton;

		if ((theButtons & Qt::MiddleButton) != 0)
			aButtons |= Aspect_VKeyMouse_MiddleButton;

		if ((theButtons & Qt::RightButton) != 0)
			aButtons |= Aspect_VKeyMouse_RightButton;

		return aButtons;
	}

	Aspect_VKeyFlags OcctQtTools::qtMouseModifiers2VKeys(Qt::KeyboardModifiers theModifiers)
	{
		Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
		if ((theModifiers & Qt::ShiftModifier) != 0)
			aFlags |= Aspect_VKeyFlags_SHIFT;

		if ((theModifiers & Qt::ControlModifier) != 0)
			aFlags |= Aspect_VKeyFlags_CTRL;

		if ((theModifiers & Qt::AltModifier) != 0)
			aFlags |= Aspect_VKeyFlags_ALT;

		return aFlags;
	}


    Aspect_VKey OcctQtTools::qtKey2VKey(int theKey)
    {
        switch (theKey)
        {
        case 1060:
        case Qt::Key_A:
            return Aspect_VKey_A;
        case 1048:
        case Qt::Key_B:
            return Aspect_VKey_B;
        case 1057:
        case Qt::Key_C:
            return Aspect_VKey_C;
        case 1042:
        case Qt::Key_D:
            return Aspect_VKey_D;
        case 1059:
        case Qt::Key_E:
            return Aspect_VKey_E;
        case 1040:
        case Qt::Key_F:
            return Aspect_VKey_F;
        case Qt::Key_G:
            return Aspect_VKey_G;
        case Qt::Key_H:
            return Aspect_VKey_H;
        case Qt::Key_I:
            return Aspect_VKey_I;
        case Qt::Key_J:
            return Aspect_VKey_J;
        case Qt::Key_K:
            return Aspect_VKey_K;
        case 1044:
        case Qt::Key_L:
            return Aspect_VKey_L;
        case Qt::Key_M:
            return Aspect_VKey_M;
        case Qt::Key_N:
            return Aspect_VKey_N;
        case Qt::Key_O:
            return Aspect_VKey_O;
        case Qt::Key_P:
            return Aspect_VKey_P;
        case 1049:
        case Qt::Key_Q:
            return Aspect_VKey_Q;
        case 1050:
        case Qt::Key_R:
            return Aspect_VKey_R;
        case 1067:
        case Qt::Key_S:
            return Aspect_VKey_S;
        case 1045:
        case Qt::Key_T:
            return Aspect_VKey_T;
        case Qt::Key_U:
            return Aspect_VKey_U;
        case 1052:
        case Qt::Key_V:
            return Aspect_VKey_V;
        case 1062:
        case Qt::Key_W:
            return Aspect_VKey_W;
        case 1063:
        case Qt::Key_X:
            return Aspect_VKey_X;
        case Qt::Key_Y:
            return Aspect_VKey_Y;
        case 1071:
        case Qt::Key_Z:
            return Aspect_VKey_Z;
            //
        case Qt::Key_0:
            return Aspect_VKey_0;
        case Qt::Key_1:
            return Aspect_VKey_1;
        case Qt::Key_2:
            return Aspect_VKey_2;
        case Qt::Key_3:
            return Aspect_VKey_3;
        case Qt::Key_4:
            return Aspect_VKey_4;
        case Qt::Key_5:
            return Aspect_VKey_5;
        case Qt::Key_6:
            return Aspect_VKey_6;
        case Qt::Key_7:
            return Aspect_VKey_7;
        case Qt::Key_8:
            return Aspect_VKey_8;
        case Qt::Key_9:
            return Aspect_VKey_9;
            //
        case Qt::Key_F1:
            return Aspect_VKey_F1;
        case Qt::Key_F2:
            return Aspect_VKey_F2;
        case Qt::Key_F3:
            return Aspect_VKey_F3;
        case Qt::Key_F4:
            return Aspect_VKey_F4;
        case Qt::Key_F5:
            return Aspect_VKey_F5;
        case Qt::Key_F6:
            return Aspect_VKey_F6;
        case Qt::Key_F7:
            return Aspect_VKey_F7;
        case Qt::Key_F8:
            return Aspect_VKey_F8;
        case Qt::Key_F9:
            return Aspect_VKey_F9;
        case Qt::Key_F10:
            return Aspect_VKey_F10;
        case Qt::Key_F11:
            return Aspect_VKey_F11;
        case Qt::Key_F12:
            return Aspect_VKey_F12;
            //
        case Qt::Key_Up:
            return Aspect_VKey_Up;
        case Qt::Key_Left:
            return Aspect_VKey_Left;
        case Qt::Key_Right:
            return Aspect_VKey_Right;
        case Qt::Key_Down:
            return Aspect_VKey_Down;
        case Qt::Key_Plus:
            return Aspect_VKey_Plus;
        case Qt::Key_Minus:
            return Aspect_VKey_Minus;
        case Qt::Key_Equal:
            return Aspect_VKey_Equal;
        case Qt::Key_PageDown:
            return Aspect_VKey_PageDown;
        case Qt::Key_PageUp:
            return Aspect_VKey_PageUp;
        case Qt::Key_Home:
            return Aspect_VKey_Home;
        case Qt::Key_End:
            return Aspect_VKey_End;
        case Qt::Key_Escape:
            return Aspect_VKey_Escape;
        case Qt::Key_Back:
            return Aspect_VKey_Back;
        case Qt::Key_Enter:
            return Aspect_VKey_Enter;
        case Qt::Key_Backspace:
            return Aspect_VKey_Backspace;
        case Qt::Key_Space:
            return Aspect_VKey_Space;
        case Qt::Key_Delete:
            return Aspect_VKey_Delete;
        case Qt::Key_Tab:
            return Aspect_VKey_Tab;
        case 1025:
        case Qt::Key_QuoteLeft:
            return Aspect_VKey_Tilde;
            //
        case Qt::Key_Shift:
            return Aspect_VKey_Shift;
        case Qt::Key_Control:
            return Aspect_VKey_Control;
        case Qt::Key_Alt:
            return Aspect_VKey_Alt;
        case Qt::Key_Menu:
            return Aspect_VKey_Menu;
        case Qt::Key_Meta:
            return Aspect_VKey_Meta;
        default:
            return Aspect_VKey_UNKNOWN;
        }
    }
    QString extStrToQtStr(const TCollection_ExtendedString& str)
    {
        std::string u8;
        u8.resize(str.LengthOfCString());
        char* u8Data = u8.data();
        str.ToUTF8CString(u8Data);
        return QString(u8.c_str());
    }
} // namespace OcctQtTools

namespace OcctGlTools
{
    Handle(OpenGl_Context) GetGlContext(const Handle(V3d_View)& theView)
    {
        Handle(OpenGl_View) aGlView = Handle(OpenGl_View)::DownCast(theView->View());
        return aGlView->GlWindow()->GetGlContext();
    }
} // namespace OcctGlTools