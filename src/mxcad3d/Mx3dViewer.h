/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <Standard_WarningsDisable.hxx>
#include <QOpenGLWidget>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_View.hxx>
#include <Standard_Version.hxx>
#include <Image_AlienPixMap.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include "Mx3dThumbnail.h"




class AIS_ViewCube;

class Mx3dViewer : public QOpenGLWidget, public AIS_ViewController
{
    Q_OBJECT
public:
    Mx3dViewer(QWidget* theParent = nullptr);

    virtual ~Mx3dViewer();

    const Handle(V3d_Viewer)& Viewer() const { return myViewer; }

    const Handle(V3d_View)& View() const { return myView; }

    const Handle(AIS_InteractiveContext)& Context() const { return myContext; }

    virtual QSize minimumSizeHint() const override { return QSize(200, 200); }

    virtual QSize sizeHint() const override { return QSize(720, 480); }

    virtual void OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx, const Handle(V3d_View)& theView) override;

    template<typename Function>
    void foreachDisplayedObject(Function fn) const;

signals:
    void selectionChanged(std::vector<Handle(AIS_InteractiveObject)> objects);
public:
    Mx3d::Thumbnail createThumbnail();
public:
    void changeProjection();
    void changeShadow();
    void showEdges();
    void wireMode();
    void fitAll();

protected: // OpenGL events
    virtual void initializeGL() override;
    virtual void paintGL() override;

protected: // user input events
    virtual bool event(QEvent* theEvent) override;
    virtual void closeEvent(QCloseEvent* theEvent) override;
    virtual void keyPressEvent(QKeyEvent* theEvent) override;
    virtual void mousePressEvent(QMouseEvent* theEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent* theEvent) override;
    virtual void mouseMoveEvent(QMouseEvent* theEvent) override;
    virtual void wheelEvent(QWheelEvent* theEvent) override;

private:
    void initBgColor();
    void updateView();
    virtual void handleViewRedraw(const Handle(AIS_InteractiveContext)& theCtx, const Handle(V3d_View)& theView) override;
    Handle(Aspect_Window) createVirtualWindow(const Handle(Graphic3d_GraphicDriver)& gfxDriver, int wndWidth, int wndHeight);
    Handle(V3d_View) createVirtualV3dView();
    Handle(Image_AlienPixMap) createImage(const Handle(V3d_View)& view);

    QPixmap toQPixmap(const Image_PixMap& pixmap);
    QByteArray toQByteArray(const QPixmap& pixmap, const char* format = "PNG");
    Mx3d::Thumbnail createViewThumbnail(const Handle(Image_AlienPixMap)& pixmap);
private:
    Handle(V3d_Viewer)             myViewer;
    Handle(V3d_View)               myView;
    Handle(AIS_InteractiveContext) myContext;
    Handle(AIS_ViewCube)           myViewCube;

    Handle(V3d_View) myFocusView;

    QString myGlInfo;
    bool    myIsCoreProfile = true;
    bool    myHasTouchInput = false;
    bool    myIsWireMode = false;
};

template<typename Function>
inline void Mx3dViewer::foreachDisplayedObject(Function fn) const
{
    AIS_ListOfInteractive listObject;
    myContext->DisplayedObjects(listObject);
    for (const Handle(AIS_InteractiveObject)& ptr : listObject)
    {
        if(ptr->IsInstance(STANDARD_TYPE(AIS_ViewCube)))
            continue;

        fn(ptr);
    }
}
