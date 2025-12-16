/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include "Mx2dICommand.h"
#include <stack>
#include <memory>
#include <QString>

class Mx2dAnnotationEditor {
public:
	Mx2dAnnotationEditor() = default;
	~Mx2dAnnotationEditor() = default;

	void loadFromFile(const QString& fileName);

	void showAllAnnotations(bool show);

	void addAnnotation(McDbObjectId id);
	void addAnnotations(McDbObjectIdArray ids);


	void eraseAnnotation(McDbObjectId id);
	void eraseAnnotations(McDbObjectIdArray ids);

	void eraseAllAnnotations();

	void moveAnnotation(McDbObjectId id, const McGeVector3d& moveVec);
	void moveAnnotations(McDbObjectIdArray ids, const McGeVector3d& moveVec);


	void modifyText(McDbObjectId id, const QString& text);

	void modifyPolyArea(McDbObjectId id, const McGePoint3dArray& oldPoints, const McGePoint3dArray& newPoints);
	void modifyArcPolyArea(McDbObjectId id, const Mx2d::PLVertexList& oldPoints, const Mx2d::PLVertexList& newPoints);
	void modifyRectArea(McDbObjectId id, const McGePoint3d& oldCorner1, const McGePoint3d& oldCorner2, const McGePoint3d& newCorner1, const McGePoint3d& newCorner2);

	void modifyAnnotation(McDbObjectId id, const QJsonObject& oldJson, const QJsonObject& newJson);

public:
	bool undo();
	bool redo();

private:
	void executeCommand(std::unique_ptr<Mx2dICommand> command);
	void saveToFile(const QString& fileName);
private:
	QString m_fileName;
	std::set<McDbObjectId> m_annotations;
	std::stack<std::unique_ptr<Mx2dICommand>> m_undoStack;
	std::stack<std::unique_ptr<Mx2dICommand>> m_redoStack;
};

