/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include "MxCADInclude.h"
#include <vector>
#include <memory>
#include <QString>
#include "Mx2dUtils.h"
#include <QJsonObject>

class Mx2dICommand
{
public:
    virtual ~Mx2dICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class Mx2dAddAnnotationCommand : public Mx2dICommand
{
public:
    Mx2dAddAnnotationCommand(McDbObjectId id);
    void execute() override;
    void undo() override;

private:
    McDbObjectId m_id;
};

class Mx2dAddAnnotationsCommand : public Mx2dICommand
{
public:
    Mx2dAddAnnotationsCommand(McDbObjectIdArray ids);
    void execute() override;
    void undo() override;

private:
    McDbObjectIdArray m_ids;
};

class Mx2dEraseAnnotationCommand : public Mx2dICommand
{
public:
    Mx2dEraseAnnotationCommand(McDbObjectId id);
    void execute() override;
    void undo() override;

private:
    McDbObjectId m_id;
};

class Mx2dEraseAnnotationsCommand : public Mx2dICommand
{
public:
    Mx2dEraseAnnotationsCommand(McDbObjectIdArray ids);
    void execute() override;
    void undo() override;

private:
    McDbObjectIdArray m_ids;
};

class Mx2dMoveAnnotationCommand : public Mx2dICommand
{
public:
    Mx2dMoveAnnotationCommand(McDbObjectId id, const McGeVector3d& moveVec);
    void execute() override;
    void undo() override;

private:
    McDbObjectId m_id;
    McGeVector3d m_moveVec;
};

class Mx2dMoveAnnotationsCommand : public Mx2dICommand
{
public:
    Mx2dMoveAnnotationsCommand(McDbObjectIdArray ids, const McGeVector3d& moveVec);
    void execute() override;
    void undo() override;

private:
    McDbObjectIdArray m_ids;
    McGeVector3d m_moveVec;
};

class Mx2dModifyTextCommand : public Mx2dICommand
{
public:
    Mx2dModifyTextCommand(McDbObjectId id, const QString& newText);
    void execute() override;
    void undo() override;

private:
    McDbObjectId m_id;
    QString m_oldText;
    QString m_newText;
};

class Mx2dModifyPolyAreaCommand : public Mx2dICommand
{
public:
    Mx2dModifyPolyAreaCommand(McDbObjectId id, const McGePoint3dArray& oldPoints, const McGePoint3dArray& newPoints);
    void execute() override;
    void undo() override;

private:
    McDbObjectId m_id;
    McGePoint3dArray m_oldPoints;
    McGePoint3dArray m_newPoints;
};

class Mx2dModifyArcPolyAreaCommand : public Mx2dICommand
{
public:
    Mx2dModifyArcPolyAreaCommand(McDbObjectId id, const Mx2d::PLVertexList& oldPoints, const Mx2d::PLVertexList& newPoints);
    void execute() override;
    void undo() override;

private:
    McDbObjectId m_id;
    Mx2d::PLVertexList m_oldPoints;
    Mx2d::PLVertexList m_newPoints;
};


class Mx2dModifyRectAreaCommand : public Mx2dICommand
{
public:
    Mx2dModifyRectAreaCommand(McDbObjectId id, const McGePoint3d& oldCorner1, const McGePoint3d& oldCorner2, const McGePoint3d& newCorner1, const McGePoint3d& newCorner2);
    void execute() override;
    void undo() override;

private:
    McDbObjectId m_id;
    McGePoint3d m_oldCorner1;
    McGePoint3d m_oldCorner2;
    McGePoint3d m_newCorner1;
    McGePoint3d m_newCorner2;
};

class Mx2dModifyAnnotationCommand : public Mx2dICommand
{
public:
    Mx2dModifyAnnotationCommand(McDbObjectId id, const QJsonObject& oldJson, const QJsonObject& newJson);
    void execute() override;
    void undo() override;

private:
    McDbObjectId m_id;
    QJsonObject m_oldJson;
    QJsonObject m_newJson;
};