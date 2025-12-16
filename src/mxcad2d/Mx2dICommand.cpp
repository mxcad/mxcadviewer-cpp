/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dICommand.h"
#include "Mx2dCustomText.h"
#include "Mx2dCustomMText.h"
#include "Mx2dCustomPolyArea.h"
#include "Mx2dCustomArcPolyArea.h"
#include "Mx2dCustomRectArea.h"

Mx2dAddAnnotationCommand::Mx2dAddAnnotationCommand(McDbObjectId id)
    : m_id(id)
{
}

void Mx2dAddAnnotationCommand::execute()
{
    McDbEntityPointer spEntity(m_id, McDb::kForWrite, true);
    spEntity->erase(false);
    MxDrawApp::UpdateDisplay();
}

void Mx2dAddAnnotationCommand::undo()
{
    McDbEntityPointer spEntity(m_id, McDb::kForWrite, true);
    spEntity->erase(true);
    MxDrawApp::UpdateDisplay();
}

Mx2dAddAnnotationsCommand::Mx2dAddAnnotationsCommand(McDbObjectIdArray ids)
    : m_ids(ids)
{
}

void Mx2dAddAnnotationsCommand::execute()
{
    for (int i = 0; i < m_ids.length(); ++i) {
        McDbEntityPointer spEntity(m_ids[i], McDb::kForWrite, true);
        spEntity->erase(false);
    }
    MxDrawApp::UpdateDisplay();
}

void Mx2dAddAnnotationsCommand::undo()
{
    for (int i = 0; i < m_ids.length(); ++i) {
        McDbEntityPointer spEntity(m_ids[i], McDb::kForWrite, true);
        spEntity->erase(true);
    }
    MxDrawApp::UpdateDisplay();
}

Mx2dEraseAnnotationCommand::Mx2dEraseAnnotationCommand(McDbObjectId id)
    : m_id(id)
{
}

void Mx2dEraseAnnotationCommand::execute()
{
    McDbEntityPointer spEntity(m_id, McDb::kForWrite, true);
    spEntity->erase(true);
    MxDrawApp::UpdateDisplay();
}

void Mx2dEraseAnnotationCommand::undo()
{
    McDbEntityPointer spEntity(m_id, McDb::kForWrite, true);
    spEntity->erase(false);
    MxDrawApp::UpdateDisplay();
}

Mx2dEraseAnnotationsCommand::Mx2dEraseAnnotationsCommand(McDbObjectIdArray ids)
    : m_ids(ids)
{
}

void Mx2dEraseAnnotationsCommand::execute()
{
    for (int i = 0; i < m_ids.length(); ++i) {
        McDbEntityPointer spEntity(m_ids[i], McDb::kForWrite, true);
        spEntity->erase(true);
    }
    MxDrawApp::UpdateDisplay();
}

void Mx2dEraseAnnotationsCommand::undo()
{
    for (int i = 0; i < m_ids.length(); ++i) {
        McDbEntityPointer spEntity(m_ids[i], McDb::kForWrite, true);
        spEntity->erase(false);
    }
    MxDrawApp::UpdateDisplay();
}

Mx2dMoveAnnotationCommand::Mx2dMoveAnnotationCommand(McDbObjectId id, const McGeVector3d& moveVec)
    : m_id(id), m_moveVec(moveVec)
{
}

void Mx2dMoveAnnotationCommand::execute()
{
    McDbEntityPointer spEntity(m_id, McDb::kForWrite, true);
    spEntity->transformBy(McGeMatrix3d::translation(m_moveVec));
    spEntity->setVisibility(McDb::kVisible);
    MxDrawApp::UpdateDisplay();
}

void Mx2dMoveAnnotationCommand::undo()
{
    McDbEntityPointer spEntity(m_id, McDb::kForWrite, true);
    spEntity->transformBy(McGeMatrix3d::translation(-m_moveVec));
    MxDrawApp::UpdateDisplay();
}

Mx2dMoveAnnotationsCommand::Mx2dMoveAnnotationsCommand(McDbObjectIdArray ids, const McGeVector3d& moveVec)
    : m_ids(ids), m_moveVec(moveVec)
{
}

void Mx2dMoveAnnotationsCommand::execute()
{
    for (int i = 0; i < m_ids.length(); ++i) {
        McDbEntityPointer spEntity(m_ids[i], McDb::kForWrite, true);
        spEntity->transformBy(McGeMatrix3d::translation(m_moveVec));
        spEntity->setVisibility(McDb::kVisible);
    }
    MxDrawApp::UpdateDisplay();
}

void Mx2dMoveAnnotationsCommand::undo()
{
    for (int i = 0; i < m_ids.length(); ++i) {
        McDbEntityPointer spEntity(m_ids[i], McDb::kForWrite, true);
        spEntity->transformBy(McGeMatrix3d::translation(-m_moveVec));
    }
    MxDrawApp::UpdateDisplay();
}

Mx2dModifyTextCommand::Mx2dModifyTextCommand(McDbObjectId id, const QString& newText)
    : m_id(id), m_newText(newText)
{
    McDbEntityPointer spEntity(m_id, McDb::kForRead, true);
    if (spEntity.openStatus() != Mcad::eOk) return;
    if (spEntity->isA() == Mx2dCustomText::desc())
    {
        Mx2dCustomText* pText = Mx2dCustomText::cast(spEntity.object());
        m_oldText = pText->textString();
    }
    else if (spEntity->isA() == Mx2dCustomMText::desc())
    {
        Mx2dCustomMText* pMText = Mx2dCustomMText::cast(spEntity.object());
        m_oldText = pMText->contents();
    }
}

void Mx2dModifyTextCommand::execute()
{
    McDbEntityPointer spEntity(m_id, McDb::kForWrite, true);
    if (spEntity.openStatus() != Mcad::eOk) return;
    if (spEntity->isA() == Mx2dCustomText::desc()) 
    {
        Mx2dCustomText* pText = Mx2dCustomText::cast(spEntity.object());
        pText->setTextString(m_newText);
    }
    else if (spEntity->isA() == Mx2dCustomMText::desc())
    {
        Mx2dCustomMText* pMText = Mx2dCustomMText::cast(spEntity.object());
        pMText->setContents(m_newText);
    }
}

void Mx2dModifyTextCommand::undo()
{
    McDbEntityPointer spEntity(m_id, McDb::kForWrite, true);
    if (spEntity.openStatus() != Mcad::eOk) return;
    if (spEntity->isA() == Mx2dCustomText::desc()) 
    {
        Mx2dCustomText* pText = Mx2dCustomText::cast(spEntity.object());
        pText->setTextString(m_oldText);
    }
    else if (spEntity->isA() == Mx2dCustomMText::desc())
    {
        Mx2dCustomMText* pMText = Mx2dCustomMText::cast(spEntity.object());
        pMText->setContents(m_oldText);
    }
}

Mx2dModifyPolyAreaCommand::Mx2dModifyPolyAreaCommand(McDbObjectId id, const McGePoint3dArray& oldPoints, const McGePoint3dArray& newPoints)
    : m_id(id), m_oldPoints(oldPoints), m_newPoints(newPoints)
{
}

void Mx2dModifyPolyAreaCommand::execute()
{
    McDbObjectPointer<Mx2dCustomPolyArea> spPolyArea(m_id, McDb::kForWrite, true);
    if (spPolyArea.openStatus() != Mcad::eOk) return;
    spPolyArea->setPoints(m_newPoints);
}

void Mx2dModifyPolyAreaCommand::undo()
{
    McDbObjectPointer<Mx2dCustomPolyArea> spPolyArea(m_id, McDb::kForWrite, true);
    if (spPolyArea.openStatus() != Mcad::eOk) return;
    spPolyArea->setPoints(m_oldPoints);
}

Mx2dModifyArcPolyAreaCommand::Mx2dModifyArcPolyAreaCommand(McDbObjectId id, const Mx2d::PLVertexList& oldPoints, const Mx2d::PLVertexList& newPoints)
    : m_id(id), m_oldPoints(oldPoints), m_newPoints(newPoints)
{
}

void Mx2dModifyArcPolyAreaCommand::execute()
{
    McDbObjectPointer<Mx2dCustomArcPolyArea> spPolyArea(m_id, McDb::kForWrite, true);
    if (spPolyArea.openStatus() != Mcad::eOk) return;
    spPolyArea->setPoints(m_newPoints);
}

void Mx2dModifyArcPolyAreaCommand::undo()
{
    McDbObjectPointer<Mx2dCustomArcPolyArea> spPolyArea(m_id, McDb::kForWrite, true);
    if (spPolyArea.openStatus() != Mcad::eOk) return;
    spPolyArea->setPoints(m_oldPoints);
}

Mx2dModifyAnnotationCommand::Mx2dModifyAnnotationCommand(McDbObjectId id, const QJsonObject& oldJson, const QJsonObject& newJson)
    : m_id(id), m_oldJson(oldJson), m_newJson(newJson)
{
}

void Mx2dModifyAnnotationCommand::execute()
{
    McDbObjectPointer<Mx2dCustomAnnotation> spAnnotation(m_id, McDb::kForWrite, true);
    if (spAnnotation.openStatus() != Mcad::eOk) return;
    spAnnotation->fromJson(m_newJson);
}

void Mx2dModifyAnnotationCommand::undo()
{
    McDbObjectPointer<Mx2dCustomAnnotation> spAnnotation(m_id, McDb::kForWrite, true);
    if (spAnnotation.openStatus() != Mcad::eOk) return;
    spAnnotation->fromJson(m_oldJson);
}

Mx2dModifyRectAreaCommand::Mx2dModifyRectAreaCommand(McDbObjectId id, const McGePoint3d& oldCorner1, const McGePoint3d& oldCorner2, const McGePoint3d& newCorner1, const McGePoint3d& newCorner2)
    : m_id(id), m_oldCorner1(oldCorner1), m_oldCorner2(oldCorner2), m_newCorner1(newCorner1), m_newCorner2(newCorner2)
{
}

void Mx2dModifyRectAreaCommand::execute()
{
    McDbObjectPointer<Mx2dCustomRectArea> spRectArea(m_id, McDb::kForWrite, true);
    if (spRectArea.openStatus() != Mcad::eOk) return;
    spRectArea->setCorner1(m_newCorner1);
    spRectArea->setCorner2(m_newCorner2);
}

void Mx2dModifyRectAreaCommand::undo()
{
    McDbObjectPointer<Mx2dCustomRectArea> spRectArea(m_id, McDb::kForWrite, true);
    if (spRectArea.openStatus() != Mcad::eOk) return;
    spRectArea->setCorner1(m_oldCorner1);
    spRectArea->setCorner2(m_oldCorner2);
}
