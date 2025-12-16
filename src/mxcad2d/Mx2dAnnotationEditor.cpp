/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dAnnotationEditor.h"
#include <QFile>

#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "Mx2dAnnotationFactory.h"

void Mx2dAnnotationEditor::loadFromFile(const QString& fileName)
{
	m_fileName = fileName;
	QFile jsonFile(fileName);
	if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Failed to open file: " << fileName;
		return;
	}
	QByteArray jsonData = jsonFile.readAll();
	jsonFile.close();
	QJsonParseError parseError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

	if (parseError.error != QJsonParseError::NoError) {
		qDebug() << "Failed to parse JSON: " << parseError.errorString();
		return;
	}
	if (!jsonDoc.isObject()) {
		qDebug() << "JSON is not an object";
		return;
	}

	QJsonObject rootObj = jsonDoc.object();
	if (!rootObj.contains("annotations") || !rootObj["annotations"].isArray()) {
		qDebug() << "JSON does not contain an array of annotations";
		return;
	}
	QJsonArray annotations = rootObj["annotations"].toArray();
	for (const auto& annotation : annotations)
	{
		if (!annotation.isObject()) {
			qDebug() << "Annotation is not an object";
			continue;
		}
		QJsonObject annotationObj = annotation.toObject();
		if (!annotationObj.contains("type")) {
			qDebug() << "Annotation does not contain a type";
			continue;
		}
		QString type = annotationObj["type"].toString();
		Mx2dCustomAnnotation* pAnnotation = Mx2dAnnotationFactory::instance().createAnnotation(type);
		if (pAnnotation)
		{
			pAnnotation->fromJson(annotationObj);
			McCmColor newColor; newColor.setRGB(230, 81, 0);
			pAnnotation->setColor(newColor);
			McDbObjectId id;
			McDbBlockTableRecordPointer spModelSpace(MCDB_MODEL_SPACE, acdbCurDwg(), McDb::kForWrite);
			if (spModelSpace.openStatus() == Mcad::eOk) {
				spModelSpace->appendAcDbEntity(id, pAnnotation);
				pAnnotation->close();
				McDbEntityPointer spEntity(id, McDb::kForWrite);
				spEntity->setLayer("MxCADAnnotationLayer");
				m_annotations.insert(id);
			}
			else {
				delete pAnnotation;
			}
		}

	}
	MxDrawApp::UpdateDisplay();
}

void Mx2dAnnotationEditor::showAllAnnotations(bool show)
{
	for (const auto& id : m_annotations) {
		McDbEntityPointer spEntity(id, McDb::kForWrite);
		if (spEntity.openStatus() == Mcad::eOk) {
			spEntity->setVisibility(show ? McDb::kVisible : McDb::kInvisible);
		}
	}
	MxDrawApp::UpdateDisplay();
}

void Mx2dAnnotationEditor::addAnnotation(McDbObjectId id)
{
	m_annotations.insert(id);
	executeCommand(std::make_unique<Mx2dAddAnnotationCommand>(id));
}

void Mx2dAnnotationEditor::addAnnotations(McDbObjectIdArray ids)
{
	for (int i = 0; i < ids.length(); i++) {
		m_annotations.insert(ids[i]);
	}
	executeCommand(std::make_unique<Mx2dAddAnnotationsCommand>(ids));
}


void Mx2dAnnotationEditor::eraseAnnotation(McDbObjectId id)
{
	executeCommand(std::make_unique<Mx2dEraseAnnotationCommand>(id));
}

void Mx2dAnnotationEditor::eraseAnnotations(McDbObjectIdArray ids)
{
	executeCommand(std::make_unique<Mx2dEraseAnnotationsCommand>(ids));
}


void Mx2dAnnotationEditor::eraseAllAnnotations()
{
	McDbObjectIdArray ids;
	for (const auto& id : m_annotations) {
		ids.append(id);
	}
	eraseAnnotations(ids);
}

void Mx2dAnnotationEditor::moveAnnotation(McDbObjectId id, const McGeVector3d& moveVec)
{
	executeCommand(std::make_unique<Mx2dMoveAnnotationCommand>(id, moveVec));
}

void Mx2dAnnotationEditor::moveAnnotations(McDbObjectIdArray ids, const McGeVector3d& moveVec)
{
	executeCommand(std::make_unique<Mx2dMoveAnnotationsCommand>(ids, moveVec));
}

void Mx2dAnnotationEditor::modifyText(McDbObjectId id, const QString& text)
{
	executeCommand(std::make_unique<Mx2dModifyTextCommand>(id, text));
}

void Mx2dAnnotationEditor::modifyPolyArea(McDbObjectId id, const McGePoint3dArray& oldPoints, const McGePoint3dArray& newPoints)
{
	executeCommand(std::make_unique<Mx2dModifyPolyAreaCommand>(id, oldPoints, newPoints));
}

void Mx2dAnnotationEditor::modifyArcPolyArea(McDbObjectId id, const Mx2d::PLVertexList& oldPoints, const Mx2d::PLVertexList& newPoints)
{
	executeCommand(std::make_unique<Mx2dModifyArcPolyAreaCommand>(id, oldPoints, newPoints));
}

void Mx2dAnnotationEditor::modifyRectArea(McDbObjectId id, const McGePoint3d& oldCorner1, const McGePoint3d& oldCorner2, const McGePoint3d& newCorner1, const McGePoint3d& newCorner2)
{
    executeCommand(std::make_unique<Mx2dModifyRectAreaCommand>(id, oldCorner1, oldCorner2, newCorner1, newCorner2));
}

void Mx2dAnnotationEditor::modifyAnnotation(McDbObjectId id, const QJsonObject& oldJson, const QJsonObject& newJson)
{
	executeCommand(std::make_unique<Mx2dModifyAnnotationCommand>(id, oldJson, newJson));
}


void Mx2dAnnotationEditor::executeCommand(std::unique_ptr<Mx2dICommand> command)
{
	command->execute();
	m_undoStack.push(std::move(command));
	while (!m_redoStack.empty())
	{
		m_redoStack.pop();
	}
	saveToFile(m_fileName);
}



void Mx2dAnnotationEditor::saveToFile(const QString& fileName)
{
	QFile jsonFile(fileName);
	if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return;
	}
	QJsonArray annotations;
	for (const auto& id : m_annotations)
	{
		McDbObjectPointer<Mx2dCustomAnnotation> spAnnotation(id, McDb::kForRead, true);
		if (spAnnotation.openStatus() != Mcad::eOk) {
			continue;
		}
		if (!spAnnotation->isErased()) {
			QJsonObject annotationObj = spAnnotation->toJson();
			annotations.append(annotationObj);
		}
	}
	QJsonObject rootObj;
	rootObj["annotations"] = annotations;
	QJsonDocument jsonDoc(rootObj);
	jsonFile.write(jsonDoc.toJson());
	jsonFile.close();
}

bool Mx2dAnnotationEditor::undo()
{
	if (m_undoStack.empty())
	{
		return false;
	}
	auto command = std::move(m_undoStack.top());
	m_undoStack.pop();
	command->undo();
	m_redoStack.push(std::move(command));
	saveToFile(m_fileName);
	return true;
}

bool Mx2dAnnotationEditor::redo()
{
	if (m_redoStack.empty())
	{
		return false;
	}
	auto command = std::move(m_redoStack.top());
	m_redoStack.pop();
	command->execute();
	m_undoStack.push(std::move(command));
	saveToFile(m_fileName);
	return true;
}
