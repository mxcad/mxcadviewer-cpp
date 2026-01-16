/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <memory>	
#include <vector>
#include <QObject>
#include "MxCADInclude.h"
#include "Mx2dUtils.h"
#include <QJsonObject>

class Mx2dSignalTransfer : public QObject
{
	Q_OBJECT
public:

	static Mx2dSignalTransfer& getInstance();

	Mx2dSignalTransfer(const Mx2dSignalTransfer&) = delete;
	Mx2dSignalTransfer& operator=(const Mx2dSignalTransfer&) = delete;

private:
	Mx2dSignalTransfer(QObject* parent = nullptr);
	~Mx2dSignalTransfer();

signals:

	void signalTextSearched(const QList<QPair<QString, Mx2d::Mx2dExtents>>& result);
	void signalLayerInformation(const Mx2d::LayerInfo& info); 
	void signalLayersOffStatusSetted(QWidget* target); 
	void signalExtractTextFinished(QWidget* target, const QStringList& textList); 
	void signalAreaMarkInformation(QWidget* target, const Mx2d::PLVertexList& arr); 
	void signalTextModify(QWidget* target, const QString& oldStr, McDbObjectId id); 
	void signalMTextModify(QWidget* target, const QString& oldStr, McDbObjectId id); 
	void signalAreaMarkId(QWidget* target, McDbObjectId id); 

	void signalDoTest(QWidget* target);
	void signalUndo(QWidget* target);
	void signalRedo(QWidget* target);
	void signalShowAllAnnotations(QWidget* target, bool show);

	void signalAddAnnotation(QWidget* target, McDbObjectId id);
	void signalAddAnnotations(QWidget* target, McDbObjectIdArray ids);

	void signalEeaseAnnotation(QWidget* target, McDbObjectId id);
	void signalEeaseAnnotations(QWidget* target, McDbObjectIdArray ids);

	void signalEraseAllAnnotations(QWidget* target);

	void signalMoveAnnotation(QWidget* target, McDbObjectId id, McGeVector3d& moveVec);
	void signalMoveAnnotations(QWidget* target, McDbObjectIdArray ids, McGeVector3d& moveVec);

	void signalModifyText(QWidget* target, McDbObjectId id, const QString& text);
	void signalModifyPolyArea(QWidget* target, McDbObjectId id, const McGePoint3dArray& oldPoints, const McGePoint3dArray& newPoints);
	void signalModifyArcPolyArea(QWidget* target, McDbObjectId id, const Mx2d::PLVertexList& oldPoints, const Mx2d::PLVertexList& newPoints);
	void signalModifyRectArea(QWidget* target, McDbObjectId id, const McGePoint3d& oldCorner1, const McGePoint3d& oldCorner2, const McGePoint3d& newCorner1, const McGePoint3d& newCorner2);
	void signalModifyAnnotation(QWidget* target, McDbObjectId id, const QJsonObject& oldJson, const QJsonObject& newJson);
};