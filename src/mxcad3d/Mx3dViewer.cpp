/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#ifdef _WIN32
  // should be included before other headers to avoid missing definitions
#include <windows.h>
#include <WNT_WClass.hxx>
#include <WNT_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif
#include <OpenGl_Context.hxx>

#include "Mx3dViewer.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QMessageBox>
#include <QMouseEvent>
#include <Standard_WarningsRestore.hxx>

#include <AIS_Shape.hxx>
#include <AIS_ViewCube.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Message.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <gsl/util>
#include <QBuffer>
#include "Mx3dUtils.h"
#include "Mx3dBgColorManager.h"
#include "Mx3dSignalTransfer.h"

//! OpenGL FBO subclass for wrapping FBO created by Qt using GL_RGBA8
//! texture format instead of GL_SRGB8_ALPHA8.
//! This FBO is set to OpenGl_Context::SetDefaultFrameBuffer() as a final target.
//! Subclass calls OpenGl_Context::SetFrameBufferSRGB() with sRGB=false flag,
//! which asks OCCT to disable GL_FRAMEBUFFER_SRGB and apply sRGB gamma correction manually.
class OcctQtFrameBuffer : public OpenGl_FrameBuffer
{
    DEFINE_STANDARD_RTTI_INLINE(OcctQtFrameBuffer, OpenGl_FrameBuffer)
public:
    //! Empty constructor.
    OcctQtFrameBuffer() {}

    //! Make this FBO active in context.
    virtual void BindBuffer(const Handle(OpenGl_Context)& theGlCtx) override
    {
        OpenGl_FrameBuffer::BindBuffer(theGlCtx);
        theGlCtx->SetFrameBufferSRGB(true, false);
    }

    //! Make this FBO as drawing target in context.
    virtual void BindDrawBuffer(const Handle(OpenGl_Context)& theGlCtx) override
    {
        OpenGl_FrameBuffer::BindDrawBuffer(theGlCtx);
        theGlCtx->SetFrameBufferSRGB(true, false);
    }

    //! Make this FBO as reading source in context.
    virtual void BindReadBuffer(const Handle(OpenGl_Context)& theGlCtx) override
    {
        OpenGl_FrameBuffer::BindReadBuffer(theGlCtx);
    }
};


Mx3dViewer::Mx3dViewer(QWidget* theParent)
    : QOpenGLWidget(theParent)
{
    Handle(Aspect_DisplayConnection) aDisp = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver)     aDriver = new OpenGl_GraphicDriver(aDisp, false);
    // lets QOpenGLWidget to manage buffer swap
    aDriver->ChangeOptions().buffersNoSwap = true;
    // don't write into alpha channel
    aDriver->ChangeOptions().buffersOpaqueAlpha = true;
    // offscreen FBOs should be always used
    aDriver->ChangeOptions().useSystemBuffer = false;

    // create viewer
    myViewer = new V3d_Viewer(aDriver);
    myViewer->SetDefaultTypeOfView(V3d_PERSPECTIVE);
    myViewer->SetDefaultLights();
    myViewer->SetLightOn();
    

    // create AIS context
    myContext = new AIS_InteractiveContext(myViewer);

    Handle(Prs3d_Drawer) highlightDrawer = new Prs3d_Drawer();

    highlightDrawer->SetColor(Quantity_Color(19 / 255.0, 107 / 255.0, 191 / 255.0, Quantity_TypeOfColor::Quantity_TOC_sRGB));
    highlightDrawer->SetDisplayMode(AIS_Shaded);

    myContext->SetHighlightStyle(Prs3d_TypeOfHighlight::Prs3d_TypeOfHighlight_Selected, highlightDrawer);

    myViewCube = new AIS_ViewCube();
    myViewCube->SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_TriedronPers, Aspect_TOTP_RIGHT_LOWER, Graphic3d_Vec2i(90, 100)));
    myViewCube->SetSize(50);
    myViewCube->SetDrawAxes(Standard_False);
    myViewCube->BoxCornerStyle()->SetColor(Quantity_NOC_WHITE);
    myViewCube->SetFontHeight(15.0);
    myViewCube->SetTextColor(Quantity_NOC_BLACK);
    myViewCube->SetViewAnimation(myViewAnimation);
    myViewCube->SetFixedAnimationLoop(false);
    myViewCube->SetAutoStartAnimation(true);
    myViewCube->SetBoxSideLabel(V3d_TypeOfOrientation::V3d_TypeOfOrientation_Zup_Front, tr("Front").toStdString().c_str());
    myViewCube->SetBoxSideLabel(V3d_TypeOfOrientation::V3d_TypeOfOrientation_Zup_Back, tr("Back").toStdString().c_str());
    myViewCube->SetBoxSideLabel(V3d_TypeOfOrientation::V3d_TypeOfOrientation_Zup_Left, tr("Left").toStdString().c_str());
    myViewCube->SetBoxSideLabel(V3d_TypeOfOrientation::V3d_TypeOfOrientation_Zup_Right, tr("Right").toStdString().c_str());
    myViewCube->SetBoxSideLabel(V3d_TypeOfOrientation::V3d_TypeOfOrientation_Zup_Top, tr("Top").toStdString().c_str());
    myViewCube->SetBoxSideLabel(V3d_TypeOfOrientation::V3d_TypeOfOrientation_Zup_Bottom, tr("Bottom").toStdString().c_str());
    Graphic3d_MaterialAspect aMat;
    aMat.SetAmbientColor(Quantity_Color(0.8f, 0.8f, 0.8f, Quantity_TypeOfColor::Quantity_TOC_sRGB));
    myViewCube->SetMaterial(aMat);

    // note - window will be created later within initializeGL() callback!
    myView = myViewer->CreateView();
    myView->SetImmediateUpdate(false);
    myView->ChangeRenderingParams().NbMsaaSamples = 4; // warning - affects performance
    myView->ChangeRenderingParams().ToShowStats = false;
    myView->ChangeRenderingParams().CollectedStats = (Graphic3d_RenderingParams::PerfCounters)(Graphic3d_RenderingParams::PerfCounters_FrameRate | Graphic3d_RenderingParams::PerfCounters_Triangles);

    myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.15, V3d_ZBUFFER);
    myView->Trihedron()->OriginAspect()->SetColor(Quantity_NOC_YELLOW);
    myView->Trihedron()->LabelAspect(V3d_TypeOfAxe::V3d_X)->SetColor(Quantity_NOC_WHITE);
    myView->Trihedron()->LabelAspect(V3d_TypeOfAxe::V3d_Y)->SetColor(Quantity_NOC_WHITE);
    myView->Trihedron()->LabelAspect(V3d_TypeOfAxe::V3d_Z)->SetColor(Quantity_NOC_WHITE);
    initBgColor();
    connect(&Mx3dSignalTransfer::getInstance(), &Mx3dSignalTransfer::signalViewSetBgColor, this, &Mx3dViewer::initBgColor);
    // Qt widget setup
    setAttribute(Qt::WA_AcceptTouchEvents); // necessary to recieve QTouchEvent events
    setMouseTracking(true);
    setBackgroundRole(QPalette::NoRole); // or NoBackground
    setFocusPolicy(Qt::StrongFocus);     // set focus policy to threat QContextMenuEvent from keyboard
    setUpdatesEnabled(true);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setCursor(QPixmap(":/resources/images3d/cross.svg"));
    // OpenGL setup managed by Qt
    QSurfaceFormat aGlFormat;
    aGlFormat.setDepthBufferSize(24);
    aGlFormat.setStencilBufferSize(8);
    // aGlFormat.setOption (QSurfaceFormat::DebugContext, true);
    aDriver->ChangeOptions().contextDebug = aGlFormat.testOption(QSurfaceFormat::DebugContext);
    // aGlFormat.setOption (QSurfaceFormat::DeprecatedFunctions, true);
    if (myIsCoreProfile)
        aGlFormat.setVersion(4, 5);

    aGlFormat.setProfile(myIsCoreProfile ? QSurfaceFormat::CoreProfile : QSurfaceFormat::CompatibilityProfile);

    // request sRGBColorSpace colorspace to meet OCCT expectations or use OcctQtFrameBuffer fallback.
    /*#if (QT_VERSION_MAJOR > 5) || (QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR >= 10)
      aGlFormat.setColorSpace(QSurfaceFormat::sRGBColorSpace);
      setTextureFormat(GL_SRGB8_ALPHA8);
    #else
      Message::SendWarning("Warning! Qt 5.10+ is required for sRGB setup.\n"
                           "Colors in 3D Viewer might look incorrect (Qt " QT_VERSION_STR " is used).\n");
      aDriver->ChangeOptions().sRGBDisable = true;
    #endif*/

    setFormat(aGlFormat);

#if defined(_WIN32)
    // never use ANGLE on Windows, since OCCT 3D Viewer does not expect this
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    // QCoreApplication::setAttribute (Qt::AA_UseOpenGLES);
#endif
}

// ================================================================
// Function : ~Mx3dViewer
// ================================================================
Mx3dViewer::~Mx3dViewer()
{
    // hold on X11 display connection till making another connection active by glXMakeCurrent()
    // to workaround sudden crash in QOpenGLWidget destructor
    Handle(Aspect_DisplayConnection) aDisp = myViewer->Driver()->GetDisplayConnection();

    // release OCCT viewer
    myContext->RemoveAll(false);
    myContext.Nullify();
    myView->Remove();
    myView.Nullify();
    myViewer.Nullify();

    // make active OpenGL context created by Qt
    makeCurrent();
    aDisp.Nullify();
}





void Mx3dViewer::OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx, const Handle(V3d_View)& theView)
{
    std::vector<Handle(AIS_InteractiveObject)> vecSelectedObjects;
    for (myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
    {
        Handle(AIS_InteractiveObject) anAisObj = myContext->SelectedInteractive();
        vecSelectedObjects.emplace_back(anAisObj);
    }
    emit selectionChanged(vecSelectedObjects);
}

Mx3d::Thumbnail Mx3dViewer::createThumbnail()
{
    myContext->Erase(myViewCube, false);
    Handle(V3d_View) view = createVirtualV3dView();
    Handle(Image_AlienPixMap) pixmap = createImage(view);
    myContext->Display(myViewCube, true);
    return createViewThumbnail(pixmap);
}

void Mx3dViewer::changeProjection()
{
    auto camera = myView->Camera();
    if (camera->ProjectionType() == Graphic3d_Camera::Projection::Projection_Perspective)
        camera->SetProjectionType(Graphic3d_Camera::Projection::Projection_Orthographic);
    else
        camera->SetProjectionType(Graphic3d_Camera::Projection::Projection_Perspective);
    myView->Redraw();
}

void Mx3dViewer::changeShadow()
{
    auto lights = myView->ActiveLights();
    for (auto light : lights) {
        auto type = light->Type();
        if (V3d_DIRECTIONAL == type)
        {
            auto isOn = light->ToCastShadows();
            light->SetCastShadows(!isOn);
        }
    }
    myView->Redraw();
}

void Mx3dViewer::showEdges()
{
    AIS_ListOfInteractive aList;
    myContext->ObjectsInside(aList);
    for (AIS_ListOfInteractive::Iterator it(aList); it.More(); it.Next())
    {
        auto& theIO = it.Value();
        if (theIO->IsInstance(STANDARD_TYPE(AIS_ViewCube))) continue;
        const bool isdraw = theIO->Attributes()->FaceBoundaryDraw();
        theIO->Attributes()->SetFaceBoundaryDraw(!isdraw);
        theIO->Redisplay();
        QApplication::processEvents();
    }
    myView->Redraw();
}

void Mx3dViewer::wireMode()
{
    AIS_DisplayMode mode = AIS_Shaded;
    if (!myIsWireMode) {
        mode = AIS_WireFrame;
    }
    myIsWireMode = !myIsWireMode;
    AIS_ListOfInteractive aList;
    myContext->ObjectsInside(aList);
    for (AIS_ListOfInteractive::Iterator it(aList); it.More(); it.Next())
    {
        auto& theIO = it.Value();
        if (theIO->IsInstance(STANDARD_TYPE(AIS_ViewCube))) continue;
        myContext->SetDisplayMode(theIO, mode, false);
        QApplication::processEvents();
    }
    myView->Redraw();
}

void Mx3dViewer::fitAll()
{
    myView->FitAll();
    myView->ZFitAll();
    myView->Redraw();
}

namespace { 
	static Handle(Aspect_NeutralWindow) createNativeWindow(QWidget* widget)
	{
		auto window = new Aspect_NeutralWindow;
		Aspect_Drawable nativeWin = 0;
#ifdef Q_OS_WIN
		HDC  wglDevCtx = wglGetCurrentDC();
		HWND wglWin = WindowFromDC(wglDevCtx);
		nativeWin = (Aspect_Drawable)wglWin;
#else
		nativeWin = (Aspect_Drawable)widget->winId();
#endif
		window->SetNativeHandle(nativeWin);
		return window;
	}
}

void Mx3dViewer::initializeGL()
{
    const QRect           aRect = rect();
    const Graphic3d_Vec2i aViewSize(aRect.right() - aRect.left(), aRect.bottom() - aRect.top());

    Aspect_Drawable aNativeWin = (Aspect_Drawable)winId();
#ifdef _WIN32
    HDC  aWglDevCtx = wglGetCurrentDC();
    HWND aWglWin = WindowFromDC(aWglDevCtx);
    aNativeWin = (Aspect_Drawable)aWglWin;
#endif

    Handle(OpenGl_Context) aGlCtx = new OpenGl_Context();
    if (!aGlCtx->Init(myIsCoreProfile))
    {
        Message::SendFail() << "Error: OpenGl_Context is unable to wrap OpenGL context";
        QMessageBox::critical(0, "Failure", "OpenGl_Context is unable to wrap OpenGL context");
        QApplication::exit(1);
        return;
    }

    Handle(Aspect_NeutralWindow) aWindow = Handle(Aspect_NeutralWindow)::DownCast(myView->Window());
	/*if (!aWindow.IsNull())
	{
		aWindow->SetNativeHandle(aNativeWin);
		aWindow->SetSize(aViewSize.x(), aViewSize.y());
		myView->SetWindow(aWindow, aGlCtx->RenderingContext());
	}
	else
	{
		aWindow = new Aspect_NeutralWindow();
		aWindow->SetVirtual(true);
		aWindow->SetNativeHandle(aNativeWin);
		aWindow->SetSize(aViewSize.x(), aViewSize.y());
		myView->SetWindow(aWindow, aGlCtx->RenderingContext());

		myContext->Display(myViewCube, 0, 0, false);
	}*/
	if (!aWindow)
        aWindow = createNativeWindow(this);

    aWindow->SetSize(aViewSize.x(), aViewSize.y());
    myView->SetWindow(aWindow, aGlCtx->RenderingContext());
}

bool Mx3dViewer::event(QEvent* theEvent)
{
    if (myView.IsNull())
        return QOpenGLWidget::event(theEvent);

    const bool isTouch = theEvent->type() == QEvent::TouchBegin
        || theEvent->type() == QEvent::TouchUpdate
        || theEvent->type() == QEvent::TouchEnd;
    if (!isTouch)
        return QOpenGLWidget::event(theEvent);

    bool hasUpdates = false;
    const QTouchEvent* aQTouchEvent = static_cast<QTouchEvent*>(theEvent);
    for (const QTouchEvent::TouchPoint& aQTouch : aQTouchEvent->touchPoints())
    {
        const Standard_Size   aTouchId = aQTouch.id();
        const Graphic3d_Vec2d aNewPos2d(aQTouch.pos().x(), aQTouch.pos().y());
        const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i(aNewPos2d + Graphic3d_Vec2d(0.5));
        if (aQTouch.state() == Qt::TouchPointPressed
            && aNewPos2i.minComp() >= 0)
        {
            hasUpdates = true;
            AIS_ViewController::AddTouchPoint(aTouchId, aNewPos2d);
        }
        else if (aQTouch.state() == Qt::TouchPointMoved
            && AIS_ViewController::TouchPoints().Contains(aTouchId))
        {
            hasUpdates = true;
            AIS_ViewController::UpdateTouchPoint(aTouchId, aNewPos2d);
        }
        else if (aQTouch.state() == Qt::TouchPointReleased
            && AIS_ViewController::RemoveTouchPoint(aTouchId))
        {
            hasUpdates = true;
        }
    }

    myHasTouchInput = true;
    if (hasUpdates)
        updateView();

    return true;
}

void Mx3dViewer::closeEvent(QCloseEvent* theEvent)
{
    theEvent->accept();
}

void Mx3dViewer::keyPressEvent(QKeyEvent* theEvent)
{
    if (myView.IsNull())
        return;

    const Aspect_VKey aKey = OcctQtTools::qtKey2VKey(theEvent->key());
    switch (aKey)
    {
    case Aspect_VKey_Escape: {
        QApplication::exit();
        return;
    }
    case Aspect_VKey_F: {
        myView->FitAll(0.01, false);
        update();
        return;
    }
    }
    QOpenGLWidget::keyPressEvent(theEvent);
}

void Mx3dViewer::mousePressEvent(QMouseEvent* theEvent)
{
    QOpenGLWidget::mousePressEvent(theEvent);
    if (myView.IsNull())
        return;

    if (myHasTouchInput && theEvent->source() == Qt::MouseEventSynthesizedBySystem)
        return; // skip mouse events emulated by system from screen touches

    const Graphic3d_Vec2i  aPnt(theEvent->pos().x(), theEvent->pos().y());
    const Aspect_VKeyMouse aButtons = OcctQtTools::qtMouseButtons2VKeys(theEvent->buttons());
    const Aspect_VKeyFlags aFlags = OcctQtTools::qtMouseModifiers2VKeys(theEvent->modifiers());
    if (AIS_ViewController::UpdateMouseButtons(aPnt, aButtons, aFlags, false))
        updateView();
}

void Mx3dViewer::mouseReleaseEvent(QMouseEvent* theEvent)
{
    QOpenGLWidget::mouseReleaseEvent(theEvent);
    if (myView.IsNull())
        return;

    const Graphic3d_Vec2i  aPnt(theEvent->pos().x(), theEvent->pos().y());
    const Aspect_VKeyMouse aButtons = OcctQtTools::qtMouseButtons2VKeys(theEvent->buttons());
    const Aspect_VKeyFlags aFlags = OcctQtTools::qtMouseModifiers2VKeys(theEvent->modifiers());
    if (AIS_ViewController::UpdateMouseButtons(aPnt, aButtons, aFlags, false))
        updateView();
}

void Mx3dViewer::mouseMoveEvent(QMouseEvent* theEvent)
{
    QOpenGLWidget::mouseMoveEvent(theEvent);
    if (myView.IsNull())
        return;
    updateView();
    if (myHasTouchInput && theEvent->source() == Qt::MouseEventSynthesizedBySystem)
        return; // skip mouse events emulated by system from screen touches

    const Graphic3d_Vec2i  aNewPos(theEvent->pos().x(), theEvent->pos().y());
    const Aspect_VKeyMouse aButtons = OcctQtTools::qtMouseButtons2VKeys(theEvent->buttons());
    const Aspect_VKeyFlags aFlags = OcctQtTools::qtMouseModifiers2VKeys(theEvent->modifiers());
    if (AIS_ViewController::UpdateMousePosition(aNewPos, aButtons, aFlags, false))
        updateView();
}

void Mx3dViewer::wheelEvent(QWheelEvent* theEvent)
{
    QOpenGLWidget::wheelEvent(theEvent);
    if (myView.IsNull())
        return;

    const Graphic3d_Vec2i aPos(Graphic3d_Vec2d(theEvent->position().x(), theEvent->position().y()));


    if (AIS_ViewController::UpdateZoom(Aspect_ScrollDelta(aPos, double(theEvent->angleDelta().y()) / 8.0)))
        updateView();
}


void Mx3dViewer::initBgColor()
{
    int bgColorMode, gradientMode;
    QColor bgColor, gradientBgColor1, gradientBgColor2;
    Mx3dBgColorManager::instance()->loadConfig(bgColorMode, gradientMode, bgColor, gradientBgColor1, gradientBgColor2);
    if (bgColorMode == 1)
    {
        myView->SetBgGradientColors(Quantity_Color(gradientBgColor1.redF(), gradientBgColor1.greenF(), gradientBgColor1.blueF(), Quantity_TypeOfColor::Quantity_TOC_sRGB), Quantity_Color(gradientBgColor2.redF(), gradientBgColor2.greenF(), gradientBgColor2.blueF(), Quantity_TypeOfColor::Quantity_TOC_sRGB), Aspect_GradientFillMethod(gradientMode));
    }
    else {
        myView->SetBgGradientStyle(Aspect_GradientFillMethod_None);
        myView->SetBackgroundColor(Quantity_Color(bgColor.redF(), bgColor.greenF(), bgColor.blueF(), Quantity_TypeOfColor::Quantity_TOC_sRGB));
    }
    myView->Redraw();
    update();
}

void Mx3dViewer::updateView()
{
    update();
    // if (window() != NULL) { window()->update(); }
}

void Mx3dViewer::paintGL()
{
    if (myView.IsNull() || myView->Window().IsNull())
        return;

    Aspect_Drawable aNativeWin = (Aspect_Drawable)winId();
#ifdef _WIN32
    HDC  aWglDevCtx = wglGetCurrentDC();
    HWND aWglWin = WindowFromDC(aWglDevCtx);
    aNativeWin = (Aspect_Drawable)aWglWin;
#endif
    if (myView->Window()->NativeHandle() != aNativeWin)
    {
        // workaround window recreation done by Qt on monitor (QScreen) disconnection
        Message::SendWarning() << "Native window handle has changed by QOpenGLWidget!";
        initializeGL();
        return;
    }

    // wrap FBO created by QOpenGLWidget
    // get context from this (composer) view rather than from arbitrary one
    // Handle(OpenGl_GraphicDriver) aDriver =
    //   Handle(OpenGl_GraphicDriver)::DownCast(myContext->CurrentViewer()->Driver());
    // Handle(OpenGl_Context) aGlCtx = aDriver->GetSharedContext();
    Handle(OpenGl_Context)     aGlCtx = OcctGlTools::GetGlContext(myView);
    Handle(OpenGl_FrameBuffer) aDefaultFbo = aGlCtx->DefaultFrameBuffer();
    if (aDefaultFbo.IsNull())
    {
        aDefaultFbo = new OcctQtFrameBuffer();
        aGlCtx->SetDefaultFrameBuffer(aDefaultFbo);
    }
    if (!aDefaultFbo->InitWrapper(aGlCtx))
    {
        aDefaultFbo.Nullify();
        Message::DefaultMessenger()->Send("Default FBO wrapper creation failed", Message_Fail);
        QMessageBox::critical(0, "Failure", "Default FBO wrapper creation failed");
        QApplication::exit(1);
        return;
    }

    Graphic3d_Vec2i aViewSizeOld;
    const Graphic3d_Vec2i        aViewSizeNew = aDefaultFbo->GetVPSize();
    Handle(Aspect_NeutralWindow) aWindow = Handle(Aspect_NeutralWindow)::DownCast(myView->Window());
    aWindow->Size(aViewSizeOld.x(), aViewSizeOld.y());
    if (aViewSizeNew != aViewSizeOld)
    {
        aWindow->SetSize(aViewSizeNew.x(), aViewSizeNew.y());
        myView->MustBeResized();
        myView->Invalidate();
    }

    // flush pending input events and redraw the viewer
    AIS_ViewController::FlushViewEvents(myContext, myView, true);
    myView->Redraw();
}

void Mx3dViewer::handleViewRedraw(const Handle(AIS_InteractiveContext)& theCtx,
    const Handle(V3d_View)& theView)
{
    AIS_ViewController::handleViewRedraw(theCtx, theView);
    if (myToAskNextFrame)
        updateView(); // ask more frames for animation
}

Handle(Aspect_Window) Mx3dViewer::createVirtualWindow(const Handle(Graphic3d_GraphicDriver)& gfxDriver, int wndWidth, int wndHeight)
{
#if defined(Q_OS_WIN)
    Q_UNUSED(gfxDriver);
    // Create a "virtual" WNT window being a pure WNT window redefined to be never shown
    static Handle(WNT_WClass) wClass;
    if (wClass.IsNull()) {
        auto cursor = LoadCursor(NULL, IDC_ARROW);
        wClass = new WNT_WClass("GW3D_Class", nullptr, CS_VREDRAW | CS_HREDRAW, 0, 0, cursor);
    }
    auto wnd = new WNT_Window("", wClass, WS_POPUP, 0, 0, wndWidth, wndHeight, Quantity_NOC_BLACK);
#else
    auto displayConn = gfxDriver->GetDisplayConnection();
    auto wnd = new Xw_Window(displayConn, "", 0, 0, wndWidth, wndHeight);
#endif

    wnd->SetVirtual(true);
    return wnd;
}

Handle(V3d_View) Mx3dViewer::createVirtualV3dView()
{
    Handle(V3d_View) view = myViewer->CreateView();
    view->ChangeRenderingParams().IsAntialiasingEnabled = true;
    view->ChangeRenderingParams().NbMsaaSamples = 4;
    view->SetBgGradientColors(Quantity_Color(62 / 255., 132 / 255., 126 / 255., Quantity_TypeOfColor::Quantity_TOC_sRGB), Quantity_Color(120 / 255., 172 / 255., 160 / 255., Quantity_TypeOfColor::Quantity_TOC_sRGB), Aspect_GradientFillMethod_Vertical);
    view->Camera()->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);

    auto wnd = createVirtualWindow(view->Viewer()->Driver(), 235, 165);
    view->SetWindow(wnd);

    return view;
}

Handle(Image_AlienPixMap) Mx3dViewer::createImage(const Handle(V3d_View)& view)
{
    if (view.IsNull())
        return {};

    Handle(V3d_Viewer) viewer = view->Viewer();
    auto _ = gsl::finally([=] {
        viewer->SetViewOff(view);
        });
    view->FitAll();
    Handle(Image_AlienPixMap) pixmap = new Image_AlienPixMap();
    V3d_ImageDumpOptions dumpOptions;
    dumpOptions.BufferType = Graphic3d_BT_RGB;
    view->Window()->Size(dumpOptions.Width, dumpOptions.Height);
    const bool okPixmap = view->ToPixMap(*pixmap.get(), dumpOptions);
    if (!okPixmap)
        return {};

    pixmap->SetFormat(Image_Format_RGB);
    return pixmap;
}

QPixmap Mx3dViewer::toQPixmap(const Image_PixMap& pixmap)
{
    auto fnToQImageFormat = [](Image_Format occFormat) {
        switch (occFormat) {
        case Image_Format_RGB:   return QImage::Format_RGB888;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        case Image_Format_BGR:   return QImage::Format_BGR888;
#endif
        case Image_Format_RGBA:  return QImage::Format_ARGB32;
        case Image_Format_RGBF:  return QImage::Format_RGB444;
        case Image_Format_Gray:  return QImage::Format_Grayscale8;
        case Image_Format_GrayF: return QImage::Format_Invalid;
        default: return QImage::Format_Invalid;
        }
        };
    const QImage img(
        pixmap.Data(),
        int(pixmap.Width()),
        int(pixmap.Height()),
        int(pixmap.SizeRowBytes()),
        fnToQImageFormat(pixmap.Format())
    );
    if (img.isNull())
        return {};

    return QPixmap::fromImage(img);
}

QByteArray Mx3dViewer::toQByteArray(const QPixmap& pixmap, const char* format)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, format);
    return bytes;
}

Mx3d::Thumbnail Mx3dViewer::createViewThumbnail(const Handle(Image_AlienPixMap)& pixmap)
{
    Mx3d::Thumbnail thumbnail;

    if (!pixmap) {
        return thumbnail;
    }

#if defined(Q_OS_WIN)
    Image_PixMap::SwapRgbaBgra(*pixmap);
#else
    Image_PixMap::FlipY(*pixmap);
#endif
    const QPixmap qPixmap = toQPixmap(*pixmap);
    thumbnail.imageData = toQByteArray(qPixmap);
    thumbnail.imageCacheKey = qPixmap.cacheKey();
    return thumbnail;
}


