/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx3dGuiDocument.h"
#include <QHBoxLayout>
#include <QSplitter>
#include "Mx3dViewer.h"
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <Poly_Triangulation.hxx>
#include <RWStl.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Builder.hxx>
#include <TDataStd_Name.hxx>
#include <VrmlAPI_CafReader.hxx>
#include <TDF_Tool.hxx>
#include <BRepTools.hxx>
#include <RWGltf_CafReader.hxx>
#include <RWObj_CafReader.hxx>
#include <XCAFPrs_AISObject.hxx>
#include <TDF_ChildIterator.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include "MxTreeWidget.h"
#include "MxViewToolBar.h"
#include "Mx3dExplodeSlider.h"
#include "Mx3dSectionTools.h"
#include "MxSignalTransfer.h"
#include "Mx3dProgressIndicator.h"
#include <QAction>
#include <QActionGroup>
#include <QMessageBox>
#include <thread>
#include <QDebug>
#include <QApplication>
#include <QtConcurrent>
#include <QTreeWidgetItem>
#include <unordered_set>
#include "MxRecentFileManager.h"
#include <AIS_ViewCube.hxx>
#include <AIS_TextLabel.hxx>
#include <TopoDS.hxx>
#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>
#include <AIS_Circle.hxx>
#include <Geom_Circle.hxx>
#include <AIS_Line.hxx>
#include <ElCLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

Mx3dGuiDocument::Mx3dGuiDocument(QWidget* parent)
	: QWidget(parent)
{
	initDocument();
	setupUi();
	createViewToolBar();
	connectSignals();
}

Mx3dGuiDocument::~Mx3dGuiDocument()
{
}



void Mx3dGuiDocument::setupUi()
{
	// main splitter
	m_pMainSplitter = new QSplitter(Qt::Horizontal, this);
	m_pMainSplitter->setChildrenCollapsible(false);
	// left splitter
	m_pLeftSplitter = new QSplitter(Qt::Vertical, m_pMainSplitter);

	m_pModelTree = new MxTreeWidget(m_pLeftSplitter);
	m_pModelTree->setMinimumWidth(200);
	m_pModelTree->setColumnCount(1);
	m_pModelTree->setHeaderLabel(tr("Model Tree"));
	m_pModelTree->setStyleSheet(R"(
		MxTreeWidget { 
			background-color: rgb(238,238,238);
		}
		MxTreeWidget::item { 
			padding: 2px 0; 
		}
	)");

	

	m_pPropertyTable = new MxTreeWidget(m_pLeftSplitter);
	m_pPropertyTable->setColumnCount(2);
	m_pPropertyTable->setHeaderLabels({ tr("Property"), "" });
	m_pPropertyTable->setStyleSheet(R"(
		MxTreeWidget { 
			background-color: rgb(238,238,238);
		}
		MxTreeWidget::item { 
			padding: 5px 0; 
		}
	)");

	m_pPropertyTable->setRootIsDecorated(false);

	m_pLeftSplitter->addWidget(m_pModelTree);
	m_pLeftSplitter->addWidget(m_pPropertyTable);

	m_pViewer = new Mx3dViewer(this);

	

	m_pMainSplitter->addWidget(m_pLeftSplitter);
	
	m_pMainSplitter->addWidget(m_pViewer);
	m_pMainSplitter->setStretchFactor(0, 3);
	m_pMainSplitter->setStretchFactor(1, 3);

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(m_pMainSplitter);
	setLayout(mainLayout);
}

void Mx3dGuiDocument::createViewToolBar()
{
	m_pViewToolBar = new MxViewToolBar(m_pViewer);
	m_pViewToolBar->setTranslucent(true);
	m_pViewToolBar->move(10, 10);

	auto actProj = new QAction(QIcon(":/resources/images3d/proj.svg"), tr("Projection Mode"), this);
	actProj->setCheckable(true);
	connect(actProj, &QAction::triggered, m_pViewer, &Mx3dViewer::changeProjection);
	m_pViewToolBar->addAction(actProj);


	auto actShadow = new QAction(QIcon(":/resources/images3d/shadow.svg"), tr("Enable Shadow"), this);
	actShadow->setCheckable(true);
	connect(actShadow, &QAction::triggered, m_pViewer, &Mx3dViewer::changeShadow);
	m_pViewToolBar->addAction(actShadow);

	auto actEdge = new QAction(QIcon(":/resources/images3d/showedges.svg"), tr("Show Edges"), this);
	actEdge->setCheckable(true);
	connect(actEdge, &QAction::triggered, m_pViewer, &Mx3dViewer::showEdges);
	m_pViewToolBar->addAction(actEdge);

	auto actWireMode = new QAction(QIcon(":/resources/images3d/wireframe.svg"), tr("Wireframe Mode"), this);
	actWireMode->setCheckable(true);
	connect(actWireMode, &QAction::triggered, m_pViewer, &Mx3dViewer::wireMode);
	m_pViewToolBar->addAction(actWireMode);

	auto actFitAll = new QAction(QIcon(":/resources/images3d/fitall.svg"), tr("Fit to Screen"), this);
	connect(actFitAll, &QAction::triggered, m_pViewer, &Mx3dViewer::fitAll);
	m_pViewToolBar->addAction(actFitAll);

	auto actModelTree = new QAction(QIcon(":/resources/images3d/modelTree.svg"), tr("Model Tree"), this);
	actModelTree->setCheckable(true);
	actModelTree->setChecked(true);
	connect(actModelTree, &QAction::triggered, this, &Mx3dGuiDocument::openModelTree);
	m_pViewToolBar->addAction(actModelTree);


	// ====================================
	m_pExplodeSlider = new Mx3dExplodeSlider(m_pViewer);
	m_pExplodeSlider->move(10, 70);
	m_pExplodeSlider->hide();
	connect(m_pExplodeSlider, &Mx3dExplodeSlider::valueChanged, this, &Mx3dGuiDocument::explodeModel);
	auto actExplode = new QAction(QIcon(":/resources/images3d/explode.svg"), tr("Exploded View"), this);
	actExplode->setCheckable(true);
	connect(actExplode, &QAction::toggled, this, [this](bool isChecked) {
		if (isChecked) m_pExplodeSlider->show();
		else m_pExplodeSlider->hide();

		});
	m_pViewToolBar->addAction(actExplode);


	// ====================================
	m_pSectionTools = new Mx3dSectionTools(m_pViewer);
	m_pSectionTools->move(10, 70);
	m_pSectionTools->hide();
	this->addClipPlanes(m_pSectionTools->getClipPlanes());
	connect(m_pSectionTools, &Mx3dSectionTools::sectionToolsChanged, this, [this]() { m_pViewer->View()->Redraw(); m_pViewer->update(); });
	auto actSection = new QAction(QIcon(":/resources/images3d/sectioning.svg"), tr("Section"), this);
	actSection->setCheckable(true);
	connect(actSection, &QAction::toggled, this, [this](bool isChecked) {
		if (isChecked) m_pSectionTools->show();
		else m_pSectionTools->hide();
		});
	m_pViewToolBar->addAction(actSection);


	// ====================================
	m_pMeasureToolBar = new MxViewToolBar(m_pViewer);
    m_pMeasureToolBar->setTranslucent(true);
	m_pMeasureToolBar->move(10, 70);
	m_pMeasureToolBar->hide();

	auto actMeasureVertex = new QAction(QIcon(":/resources/images3d/measureVertex.svg"), tr("Point Measurement"), this);
	actMeasureVertex->setCheckable(true);
    connect(actMeasureVertex, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::VertexPosition : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasureVertex);

	auto actMeasureCircleCenter = new QAction(QIcon(":/resources/images3d/measureCircleCenter.svg"), tr("Circle Center Measurement"), this);
	actMeasureCircleCenter->setCheckable(true);
    connect(actMeasureCircleCenter, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::CircleCenter : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasureCircleCenter);

	auto actMeasureDiameter = new QAction(QIcon(":/resources/images3d/measureDiameter.svg"), tr("Diameter Measurement"), this);
	actMeasureDiameter->setCheckable(true);
    connect(actMeasureDiameter, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::CircleDiameter : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasureDiameter);

	auto actMeasureMinDist = new QAction(QIcon(":/resources/images3d/measureMinDist.svg"), tr("Minimum Distance Measurement"), this);
	actMeasureMinDist->setCheckable(true);
    connect(actMeasureMinDist, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::MinDistance : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasureMinDist);

	auto actMeasure2PtMinDist = new QAction(QIcon(":/resources/images3d/measure2PtMinDist.svg"), tr("Two Center Points Distance Measurement"), this);
	actMeasure2PtMinDist->setCheckable(true);
    connect(actMeasure2PtMinDist, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::CenterDistance : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasure2PtMinDist);

	auto actMeasureAngle = new QAction(QIcon(":/resources/images3d/measureAngle.svg"), tr("Angle Measurement"), this);
	actMeasureAngle->setCheckable(true);
    connect(actMeasureAngle, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::Angle : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasureAngle);

	auto actMeasureLength = new QAction(QIcon(":/resources/images3d/measureLength.svg"), tr("Length Measurement"), this);
	actMeasureLength->setCheckable(true);
    connect(actMeasureLength, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::Length : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasureLength);

	auto actMeasureArea = new QAction(QIcon(":/resources/images3d/measureArea.svg"), tr("Surface Area Measurement"), this);
	actMeasureArea->setCheckable(true);
    connect(actMeasureArea, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::Area : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasureArea);

	auto actMeasureBoundingBox = new QAction(QIcon(":/resources/images3d/measureBoundingBox.svg"), tr("Bounding Box Measurement"), this);
	actMeasureBoundingBox->setCheckable(true);
    connect(actMeasureBoundingBox, &QAction::toggled, this, [this](bool isChecked) {
		enableSelectionMode(isChecked ? Mx3d::MeasureType::BoundingBox : Mx3d::MeasureType::None);
		});
	m_pMeasureToolBar->addAction(actMeasureBoundingBox);

	m_pActionGroup_2 = new QActionGroup(this);
	m_pActionGroup_2->addAction(actMeasureVertex);
	m_pActionGroup_2->addAction(actMeasureCircleCenter);
	m_pActionGroup_2->addAction(actMeasureDiameter);
	m_pActionGroup_2->addAction(actMeasureMinDist);
	m_pActionGroup_2->addAction(actMeasure2PtMinDist);
	m_pActionGroup_2->addAction(actMeasureAngle);
	m_pActionGroup_2->addAction(actMeasureLength);
	m_pActionGroup_2->addAction(actMeasureArea);
	m_pActionGroup_2->addAction(actMeasureBoundingBox);
	m_pActionGroup_2->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

	auto actMeasure = new QAction(QIcon(":/resources/images3d/measure.svg"), tr("Measure"), this);
	actMeasure->setCheckable(true);
	connect(actMeasure, &QAction::toggled, this, [this](bool isChecked) {
		if (isChecked) { m_pMeasureToolBar->show(); }
		else {
			m_pMeasureToolBar->hide();
			for (auto action : m_pActionGroup_2->actions())
			{
				action->setChecked(false);
			}
			stopMeasure();
		}
		});
	m_pViewToolBar->addAction(actMeasure);
    m_pActionGroup_1 = new QActionGroup(this);
	m_pActionGroup_1->addAction(actExplode);
	m_pActionGroup_1->addAction(actSection);
	m_pActionGroup_1->addAction(actMeasure);
	m_pActionGroup_1->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
	connect(m_pActionGroup_1, &QActionGroup::triggered, this, [this](QAction* action) {
		auto rValue = MxUtils::doAction([](){ /* Do Nothing! */});
		if (rValue != 2)
		{
			for (auto act : m_pActionGroup_1->actions())
			{
				act->setChecked(false);
			}
		}
		});
}

void Mx3dGuiDocument::initDocument()
{
	auto app = XCAFApp_Application::GetApplication();
	app->NewDocument("mx3d", m_spDocument);
	TDF_Label rootLabel = m_spDocument->Main();
	m_spShapeTool = XCAFDoc_DocumentTool::ShapeTool(rootLabel);
	m_spColorTool = XCAFDoc_DocumentTool::ColorTool(rootLabel);
	m_spLayerTool = XCAFDoc_DocumentTool::LayerTool(rootLabel);


	m_pProgressIndicator = new Mx3dProgressIndicator(this);
}

void Mx3dGuiDocument::connectSignals()
{
	connect(this, &Mx3dGuiDocument::fileOpened, this, &Mx3dGuiDocument::drawShapes);
	connect(m_pModelTree, &MxTreeWidget::currentItemChanged, this, &Mx3dGuiDocument::onModelTreeItemChanged);
	connect(m_pViewer, &Mx3dViewer::selectionChanged, this, [this](std::vector<Handle(AIS_InteractiveObject)> objects) {
		m_pModelTree->clearSelection();
		m_pModelTree->setCurrentItem(nullptr);
		m_pPropertyTable->clear();
		std::vector<Mx3d::TreeNodeId> vecSelectedNodes;
		for (Handle(AIS_InteractiveObject) object : objects) {
			vecSelectedNodes.emplace_back(nodeFromGraphicsObject(object));
		}

		Mx3d::TreeNodeId lastId = 0;
		std::vector<QTreeWidgetItem*> vecSelectedItems;
		bool isSelected = false;
		for (auto id : vecSelectedNodes)
		{
			if (m_mapNodeIdToTreeItem[id])
			{
				vecSelectedItems.emplace_back(m_mapNodeIdToTreeItem[id]);
				lastId = id;
				isSelected = true;
			}
		}
		if (!m_pModelTree->isHidden() && isSelected)
		{
			for (auto item : vecSelectedItems)
			{
				item->setSelected(true);
				m_pModelTree->expandItem(item);
				m_pModelTree->scrollToItem(item, QAbstractItemView::PositionAtCenter);
			}
			m_pModelTree->setFocus();
			setProperty(lastId);
		}
		if (m_isMeasuring)
		{
			switch (m_measureType)
			{
			case Mx3d::MeasureType::None:
				break;
			case Mx3d::MeasureType::VertexPosition:
				measureVertex();
				break;
			case Mx3d::MeasureType::CircleCenter:
				measureCircleCenter();
				break;
			case Mx3d::MeasureType::CircleDiameter:
				measureCircleDiameter();
				break;
			case Mx3d::MeasureType::MinDistance:
				measureMinDistance();
				break;
			case Mx3d::MeasureType::CenterDistance:
				measureCenterDistance();
				break;
			case Mx3d::MeasureType::Angle:
				measureAngle();
				break;
			case Mx3d::MeasureType::Length:
				measureLength();
				break;
			case Mx3d::MeasureType::Area:
				measureArea();
				break;
			case Mx3d::MeasureType::BoundingBox:
				measureBoundingBox();
				break;
			default:
				break;
			}
		}
		});
}

TopLoc_Location Mx3dGuiDocument::shapeReferenceLocation(const TDF_Label& lbl)
{
	return XCAFDoc_ShapeTool::GetLocation(lbl);
}

TopLoc_Location Mx3dGuiDocument::shapeAbsoluteLocation(const Mx3d::Tree<TDF_Label>& modelTree, Mx3d::TreeNodeId nodeId)
{
	TopLoc_Location absoluteLoc;
	for (Mx3d::TreeNodeId it = nodeId; it != 0; it = modelTree.nodeParent(it)) {
		const TDF_Label& nodeLabel = modelTree.nodeData(it);
		const TopLoc_Location nodeLoc = shapeReferenceLocation(nodeLabel);
		absoluteLoc = nodeLoc * absoluteLoc;
	}
	return absoluteLoc;
}

Bnd_Box Mx3dGuiDocument::aisObjectBoundingBox(const Handle(AIS_InteractiveObject)& object)
{
	Bnd_Box box;
	if (object.IsNull())
		return box;

	for (Handle(PrsMgr_Presentation) prs : object->Presentations()) {
		if (prs->Mode() == object->DisplayMode() && !prs->CStructure()->BoundingBox().IsValid())
			prs->CalculateBoundBox();
	}

	object->BoundingBox(box);
	return box;
}

void Mx3dGuiDocument::addBndBoxToEntity(Bnd_Box* box, const Bnd_Box& other)
{
	const auto bbc = Mx3d::BndBoxCoords::get(other);
	for (const gp_Pnt& pnt : bbc.vertices())
		box->Add(pnt);
}

Handle(AIS_InteractiveObject) Mx3dGuiDocument::createGraphicsObject(const TDF_Label& label)
{
	if (XCAFDoc_ShapeTool::IsShape(label)) {
		auto object = new XCAFPrs_AISObject(label);

		object->Attributes()->SetIsoOnTriangulation(false);
		object->Attributes()->SetFaceBoundaryDraw(true);
		object->Attributes()->SetFaceBoundaryAspect(
			new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.)
		);

		Graphic3d_MaterialAspect aMat;

		aMat.SetDiffuseColor(Quantity_Color(1.0, 1.0, 1.0, Quantity_TypeOfColor::Quantity_TOC_sRGB));
		aMat.SetSpecularColor(Quantity_Color(0.5, 0.5, 0.5, Quantity_TypeOfColor::Quantity_TOC_sRGB));
		aMat.SetShininess(0.9f);

		object->SetMaterial(aMat);

		object->SetDisplayMode(AIS_Shaded);

		return object;
	}

	return {};
}

Mx3d::TreeNodeId Mx3dGuiDocument::deepBuildAssemblyTree(Mx3d::TreeNodeId parentNode, const TDF_Label& label)
{
	const Mx3d::TreeNodeId node = m_modelTree.appendChild(parentNode, label);
	if (XCAFDoc_ShapeTool::IsAssembly(label)) {
		TDF_LabelSequence seq;
		XCAFDoc_ShapeTool::GetComponents(label, seq);
		for (const TDF_Label& child : seq)
			this->deepBuildAssemblyTree(node, child);
	}
	else if (XCAFDoc_ShapeTool::IsReference(label)) {
		TDF_Label referred;
		XCAFDoc_ShapeTool::GetReferredShape(label, referred);
		this->deepBuildAssemblyTree(node, referred);
	}
	return node;
}

void Mx3dGuiDocument::mapEntity(Mx3d::TreeNodeId entityTreeNodeId)
{
	GraphicsEntity gfxEntity;
	gfxEntity.treeNodeId = entityTreeNodeId;
	std::unordered_map<TDF_Label, Handle(AIS_InteractiveObject)> mapLabelGfxProduct;

	traverseTree(entityTreeNodeId, m_modelTree, [&, this](Mx3d::TreeNodeId id) {
		const TDF_Label nodeLabel = m_modelTree.nodeData(id);
		if (m_modelTree.nodeIsLeaf(id)) {
			Handle(AIS_InteractiveObject) gfxProduct = Mx3d::CppUtils::findValue(nodeLabel, mapLabelGfxProduct);
			if (!gfxProduct) {
				gfxProduct = createGraphicsObject(nodeLabel);
				if (!gfxProduct)
					return;

				mapLabelGfxProduct.insert({ nodeLabel, gfxProduct });
			}

			if (!m_modelTree.nodeIsRoot(id)) {
				const Mx3d::TreeNodeId parentNodeId = m_modelTree.nodeParent(id);
				const TDF_Label parentNodeLabel = m_modelTree.nodeData(parentNodeId);
				if (XCAFDoc_ShapeTool::IsReference(parentNodeLabel)/* && isSetColor(parentNodeLabel)*/) {
					// Parent node is a reference and it redefines color attribute, so the graphics
					// can't be shared with the product
					auto gfxObject = createGraphicsObject(parentNodeLabel);
					const Mx3d::TreeNodeId grandParentNodeId = m_modelTree.nodeParent(parentNodeId);
					const TopLoc_Location locGrandParentShape = shapeAbsoluteLocation(m_modelTree, grandParentNodeId);
					gfxObject->SetLocalTransformation(locGrandParentShape);
					gfxEntity.vecObject.push_back(gfxObject);
				}
				else {
					auto gfxInstance = new AIS_ConnectedInteractive;
					gfxInstance->Connect(gfxProduct, shapeAbsoluteLocation(m_modelTree, id));
					gfxInstance->SetDisplayMode(gfxProduct->DisplayMode());
					gfxInstance->Attributes()->SetFaceBoundaryDraw(gfxProduct->Attributes()->FaceBoundaryDraw());
					gfxInstance->SetOwner(gfxProduct->GetOwner());
					gfxEntity.vecObject.push_back(Handle(AIS_InteractiveObject)(gfxInstance));
				}

				if (XCAFDoc_ShapeTool::IsReference(parentNodeLabel))
					id = m_modelTree.nodeParent(id);
			}
			else {
				gfxEntity.vecObject.push_back(gfxProduct);
			}

			const GraphicsEntity::Object& lastGfxObject = gfxEntity.vecObject.back();
			gfxEntity.mapTreeNodeGfxObject.insert({ id, lastGfxObject.ptr });
			gfxEntity.mapGfxObjectTreeNode.insert({ lastGfxObject.ptr, id });
		}
		});

	for (const GraphicsEntity::Object& object : gfxEntity.vecObject) {
		auto aPrs = object.ptr;
		m_pViewer->Context()->Display(aPrs, false);
		QApplication::processEvents();
	}

	for (GraphicsEntity::Object& object : gfxEntity.vecObject) {
		object.bndBox = aisObjectBoundingBox(object.ptr);
		object.trsfOriginal = m_pViewer->Context()->Location(object.ptr);
		addBndBoxToEntity(&gfxEntity.bndBox, object.bndBox);
	}
	addBndBoxToEntity(&m_modelBndBox, gfxEntity.bndBox);
	
	m_vecGraphicsEntity.push_back(std::move(gfxEntity));
	traverseTree(entityTreeNodeId, m_modelTree, [=](Mx3d::TreeNodeId id) {
		m_mapTreeNodeCheckState.insert({ id, CheckState::On });
		});
}

Mx3d::TreeNodeId Mx3dGuiDocument::nodeFromGraphicsObject(const Handle(AIS_InteractiveObject)& gfxObject) const
{
	if (!gfxObject)
		return 0;

	for (const GraphicsEntity& gfxEntity : m_vecGraphicsEntity) {
		auto it = gfxEntity.mapGfxObjectTreeNode.find(gfxObject);
		if (it != gfxEntity.mapGfxObjectTreeNode.cend())
			return it->second;
	}

	return 0;
}

const TCollection_ExtendedString& Mx3dGuiDocument::labelAttrStdName(const TDF_Label& label)
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

QString Mx3dGuiDocument::referenceItemText(const TDF_Label& instanceLabel, const TDF_Label& productLabel)
{
	const QString productName = OcctQtTools::extStrToQtStr(labelAttrStdName(productLabel)).trimmed();
	return productName;
}

QIcon Mx3dGuiDocument::shapeIcon(const TDF_Label& label)
{
	if (XCAFDoc_ShapeTool::IsAssembly(label))
		return QIcon(":/resources/images3d/assembly.svg");
	else if (XCAFDoc_ShapeTool::IsSimpleShape(label))
		return QIcon(":/resources/images3d/simple.svg");

	return QIcon();
}

QTreeWidgetItem* Mx3dGuiDocument::buildModelTreeItems(QTreeWidgetItem* treeItem, const Mx3d::TreeNodeId& id)
{
	std::unordered_map<Mx3d::TreeNodeId, QTreeWidgetItem*> mapNodeIdToTreeItem;
	std::unordered_set<Mx3d::TreeNodeId> setReferenceNodeId;
	const auto& modelTree = m_modelTree;
	traverseTree(id, modelTree, [&](Mx3d::TreeNodeId itNodeId) {
		const Mx3d::TreeNodeId nodeParentId = modelTree.nodeParent(itNodeId);
		auto itParentFound = mapNodeIdToTreeItem.find(nodeParentId);
		QTreeWidgetItem* guiParentNode = itParentFound != mapNodeIdToTreeItem.end() ? itParentFound->second : treeItem;
		const TDF_Label& nodeLabel = modelTree.nodeData(itNodeId);
		if (XCAFDoc_ShapeTool::IsReference(nodeLabel)) {
			mapNodeIdToTreeItem[itNodeId] = guiParentNode;
			setReferenceNodeId.insert(itNodeId);
		}
		else {
			auto guiNode = new QTreeWidgetItem(guiParentNode);
			QString guiNodeText = OcctQtTools::extStrToQtStr(labelAttrStdName(nodeLabel));
			Mx3d::TreeNodeId guiNodeId = itNodeId;
			if (setReferenceNodeId.find(nodeParentId) != setReferenceNodeId.cend()) {
				const TDF_Label& refLabel = modelTree.nodeData(nodeParentId);
				guiNodeText = referenceItemText(refLabel, nodeLabel);
				guiNodeId = nodeParentId;
				if (!guiParentNode)
					mapNodeIdToTreeItem[nodeParentId] = guiNode;
			}

			guiNode->setText(0, guiNodeText);
			guiNode->setData(0, Qt::UserRole + 3, guiNodeId);
			m_mapNodeIdToTreeItem.insert({ guiNodeId, guiNode });
			const QIcon icon = shapeIcon(nodeLabel);
			if (!icon.isNull())
				guiNode->setIcon(0, icon);
			mapNodeIdToTreeItem[itNodeId] = guiNode;
		}
		});

	return mapNodeIdToTreeItem.find(id)->second;
}

std::vector<TopAbs_ShapeEnum> Mx3dGuiDocument::selectionModes(Mx3d::MeasureType type)
{
	switch (type) {
	case Mx3d::MeasureType::VertexPosition: {
		static const std::vector<TopAbs_ShapeEnum> modes = { TopAbs_VERTEX };
		return modes;
	}
	case Mx3d::MeasureType::CircleCenter:
	case Mx3d::MeasureType::CircleDiameter:
	case Mx3d::MeasureType::Length:
	case Mx3d::MeasureType::Angle: {
		static const std::vector<TopAbs_ShapeEnum> modes = { TopAbs_EDGE };
		return modes;
	}
	case Mx3d::MeasureType::MinDistance:
	case Mx3d::MeasureType::CenterDistance: {
		static const std::vector<TopAbs_ShapeEnum> modes = { TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE };
		return modes;
	}
	case Mx3d::MeasureType::Area: {
		static const std::vector<TopAbs_ShapeEnum> modes = { TopAbs_FACE };
		return modes;
	}
	case Mx3d::MeasureType::BoundingBox: {
		static const std::vector<TopAbs_ShapeEnum> modes = { TopAbs_COMPOUND };
		return modes;
	}
	default: {
		return {};
	}
	}
}

void Mx3dGuiDocument::stopMeasure()
{
	m_pViewer->Context()->ClearSelected(false);
	m_pViewer->foreachDisplayedObject([=](const Handle(AIS_InteractiveObject)& gfxObject) {
		m_pViewer->Context()->Deactivate(gfxObject);
		m_pViewer->Context()->Activate(gfxObject, AIS_Shape::SelectionMode(TopAbs_SHAPE));
		});
	m_isMeasuring = false;
	m_measureType = Mx3d::MeasureType::None;

	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}

	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::enableSelectionMode(Mx3d::MeasureType type)
{
	std::vector<TopAbs_ShapeEnum> modes = selectionModes(type);
	if (modes.empty())
	{
		stopMeasure();
		return;
	}
	m_pViewer->foreachDisplayedObject([=](const Handle(AIS_InteractiveObject)& gfxObject) {
		if (!Handle(AIS_ViewCube)::DownCast(gfxObject).IsNull())
			return;

		m_pViewer->Context()->Deactivate(gfxObject);
		for (int i = 0; i < modes.size(); i++) {
			m_pViewer->Context()->Activate(gfxObject, AIS_Shape::SelectionMode(modes[i]));
		}
		});
	m_isMeasuring = true;
	m_measureType = type;
}

std::vector<TopoDS_Shape> Mx3dGuiDocument::getCurSelected()
{
	std::vector<TopoDS_Shape> vec;

	for (m_pViewer->Context()->InitSelected(); m_pViewer->Context()->MoreSelected(); m_pViewer->Context()->NextSelected()) {
		TopoDS_Shape shape = m_pViewer->Context()->SelectedShape();
		vec.emplace_back(shape);
	}
	return vec;
}
void Mx3dGuiDocument::measureVertex()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}
	auto vec = getCurSelected();
	for (auto shape : vec) {
		if (shape.ShapeType() == TopAbs_VERTEX)
		{
			TopoDS_Vertex myVertex = TopoDS::Vertex(shape);
			gp_Pnt pt = BRep_Tool::Pnt(myVertex);

			Handle(AIS_TextLabel) anAisTextLabel = new AIS_TextLabel();
			anAisTextLabel->SetPosition(pt);
			QString qStr = QString("X:%1 Y:%2 Z:%3").arg(pt.X()).arg(pt.Y()).arg(pt.Z());
			anAisTextLabel->SetText(TCollection_ExtendedString(qStr.toStdString().c_str(), true));
			anAisTextLabel->SetDisplayType(Aspect_TODT_SUBTITLE);
			anAisTextLabel->SetColorSubTitle(Quantity_NOC_BLACK);
			anAisTextLabel->SetColor(Quantity_NOC_WHITE);
			anAisTextLabel->SetTransparency(0.2);
			anAisTextLabel->SetZLayer(Graphic3d_ZLayerId_Topmost);

			Handle(AIS_Point) anAisPnt = new AIS_Point(new Geom_CartesianPoint(pt));
			anAisPnt->SetZLayer(Graphic3d_ZLayerId_Topmost);
			anAisPnt->SetColor(Quantity_NOC_BLACK);

			m_vecMeasureFlags.emplace_back(anAisTextLabel);
			m_vecMeasureFlags.emplace_back(anAisPnt);

			m_pViewer->Context()->Display(anAisTextLabel, false);
			m_pViewer->Context()->Display(anAisPnt, false);
		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::measureCircleCenter()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}
	auto vec = getCurSelected();

	for (auto shape : vec) {
		if (shape.ShapeType() == TopAbs_EDGE)
		{
			Mx3d::MeasureValue mcv;
			Mx3d::MeasureCircle mc;

			if (brepCircle(shape, mc))
			{
				mcv = mc;
				if (!std::holds_alternative<Mx3d::MeasureNone>(mcv))
				{
					gp_Circ circ(mc.value);
					Handle(AIS_Point) anAisPnt = new AIS_Point(new Geom_CartesianPoint(circ.Location()));
					Handle(AIS_TextLabel) anAisTextLabel = new AIS_TextLabel();
					gp_Pnt pt = circ.Location();
					anAisTextLabel->SetPosition(pt);
					QString qStr = QString("X:%1 Y:%2 Z:%3").arg(pt.X()).arg(pt.Y()).arg(pt.Z());
					anAisTextLabel->SetText(TCollection_ExtendedString(qStr.toStdString().c_str(), true));
					Handle(AIS_Circle) anAisCircle;
					if (mc.isArc)
					{
						anAisCircle = new AIS_Circle(new Geom_Circle(circ));
					}

					anAisPnt->SetZLayer(Graphic3d_ZLayerId_Topmost);
					anAisPnt->SetColor(Quantity_NOC_BLACK);


					anAisTextLabel->SetZLayer(Graphic3d_ZLayerId_Topmost);
					anAisTextLabel->SetDisplayType(Aspect_TODT_SUBTITLE);
					anAisTextLabel->SetColorSubTitle(Quantity_NOC_BLACK);
					anAisTextLabel->SetColor(Quantity_NOC_WHITE);
					anAisTextLabel->SetTransparency(0.2);



					if (anAisCircle)
					{
						anAisCircle->SetZLayer(Graphic3d_ZLayerId_Topmost);
						anAisCircle->SetColor(Quantity_NOC_BLACK);
						anAisCircle->Attributes()->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT);

						m_vecMeasureFlags.emplace_back(anAisCircle);

						m_pViewer->Context()->Display(anAisCircle, false);
					}

					m_vecMeasureFlags.emplace_back(anAisPnt);
					m_vecMeasureFlags.emplace_back(anAisTextLabel);

					m_pViewer->Context()->Display(anAisPnt, false);
					m_pViewer->Context()->Display(anAisTextLabel, false);
				}
			}


		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::measureCircleDiameter()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}
	auto vec = getCurSelected();

	for (auto shape : vec) {
		if (shape.ShapeType() == TopAbs_EDGE)
		{
			Mx3d::MeasureValue mv;
			Mx3d::MeasureCircle mc;

			if (brepCircle(shape, mc))
			{
				mv = mc;
				if (!std::holds_alternative<Mx3d::MeasureNone>(mv))
				{
					gp_Circ circ(mc.value);

					gp_Pnt firstAnchorPnt = mc.pntAnchor;
					gp_Pnt secondAnchorPnt = firstAnchorPnt.Translated(2 * gp_Vec{ firstAnchorPnt, circ.Location() });

					Handle(AIS_Line) anAisLine = new AIS_Line(new Geom_CartesianPoint(firstAnchorPnt), new Geom_CartesianPoint(secondAnchorPnt));
					anAisLine->SetWidth(2.5);
					anAisLine->Attributes()->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT);

					Handle(AIS_TextLabel) anAisTextLabel = new AIS_TextLabel();
					anAisTextLabel->SetPosition(circ.Location());
					QString qStr = QString(u8"Ø%1").arg(2 * circ.Radius());
					anAisTextLabel->SetText(TCollection_ExtendedString(qStr.toStdString().c_str(), true));

					Handle(AIS_Circle) anAisCircle = new AIS_Circle(new Geom_Circle(circ));

					anAisTextLabel->SetZLayer(Graphic3d_ZLayerId_Topmost);
					anAisTextLabel->SetDisplayType(Aspect_TODT_SUBTITLE);
					anAisTextLabel->SetColorSubTitle(Quantity_NOC_BLACK);
					anAisTextLabel->SetColor(Quantity_NOC_WHITE);
					anAisTextLabel->SetTransparency(0.2);

					anAisCircle->SetZLayer(Graphic3d_ZLayerId_Topmost);
					anAisCircle->SetColor(Quantity_NOC_BLACK);
					anAisCircle->Attributes()->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT);

					m_vecMeasureFlags.emplace_back(anAisLine);
					m_vecMeasureFlags.emplace_back(anAisCircle);
					m_vecMeasureFlags.emplace_back(anAisTextLabel);

					m_pViewer->Context()->Display(anAisLine, false);
					m_pViewer->Context()->Display(anAisCircle, false);
					m_pViewer->Context()->Display(anAisTextLabel, false);
				}
			}


		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::measureMinDistance()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}
	auto vec = getCurSelected();

	if (vec.size() < 2)
	{
		return;
	}

	Mx3d::MeasureDistance md;
	if (brepMinDistance(vec[0], vec[1], md))
	{
		Mx3d::MeasureValue mv = md;
		if (!std::holds_alternative<Mx3d::MeasureNone>(mv))
		{
			Handle(AIS_Line) anAisLine = new AIS_Line(new Geom_CartesianPoint(md.pnt1), new Geom_CartesianPoint(md.pnt2));

			Handle(AIS_TextLabel) anAisTextLabel = new AIS_TextLabel();

			Handle(AIS_Point) anAisPnt1 = new AIS_Point(new Geom_CartesianPoint(md.pnt1));
			Handle(AIS_Point) anAisPnt2 = new AIS_Point(new Geom_CartesianPoint(md.pnt2));

			anAisLine->SetWidth(2.5);
			anAisLine->Attributes()->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT);

			anAisTextLabel->SetPosition(md.pnt1.Translated(gp_Vec(md.pnt1, md.pnt2) / 2.));
			QString qStr = QString("%1").arg(md.value);
			anAisTextLabel->SetText(TCollection_ExtendedString(qStr.toStdString().c_str(), true));

			auto applyGraphicsDefaults = [](const Handle(AIS_InteractiveObject)& obj) {
				obj->SetZLayer(Graphic3d_ZLayerId_Topmost);
				auto textLabel = Handle(AIS_TextLabel)::DownCast(obj);
				if (textLabel) {
					textLabel->SetDisplayType(Aspect_TODT_SUBTITLE);
					textLabel->SetColorSubTitle(Quantity_NOC_BLACK);
					textLabel->SetColor(Quantity_NOC_WHITE);
					textLabel->SetTransparency(0.2);
				}
				else {
					obj->SetColor(Quantity_NOC_BLACK);
				}
				};

			applyGraphicsDefaults(anAisLine);
			applyGraphicsDefaults(anAisTextLabel);
			applyGraphicsDefaults(anAisPnt1);
			applyGraphicsDefaults(anAisPnt2);

			m_vecMeasureFlags.emplace_back(anAisLine);
			m_vecMeasureFlags.emplace_back(anAisPnt1);
			m_vecMeasureFlags.emplace_back(anAisPnt2);
			m_vecMeasureFlags.emplace_back(anAisTextLabel);

			m_pViewer->Context()->Display(anAisLine, false);
			m_pViewer->Context()->Display(anAisPnt1, false);
			m_pViewer->Context()->Display(anAisPnt2, false);
			m_pViewer->Context()->Display(anAisTextLabel, false);
		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::measureCenterDistance()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}
	auto vec = getCurSelected();

	if (vec.size() < 2)
	{
		return;
	}

	Mx3d::MeasureDistance md;
	if (brepCenterDistance(vec[0], vec[1], md))
	{
		Mx3d::MeasureValue mv = md;
		if (!std::holds_alternative<Mx3d::MeasureNone>(mv))
		{
			Handle(AIS_Line) anAisLine = new AIS_Line(new Geom_CartesianPoint(md.pnt1), new Geom_CartesianPoint(md.pnt2));

			Handle(AIS_TextLabel) anAisTextLabel = new AIS_TextLabel();

			Handle(AIS_Point) anAisPnt1 = new AIS_Point(new Geom_CartesianPoint(md.pnt1));
			Handle(AIS_Point) anAisPnt2 = new AIS_Point(new Geom_CartesianPoint(md.pnt2));

			anAisLine->SetWidth(2.5);
			anAisLine->Attributes()->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT);

			anAisTextLabel->SetPosition(md.pnt1.Translated(gp_Vec(md.pnt1, md.pnt2) / 2.));
			QString qStr = QString("%1").arg(md.value);
			anAisTextLabel->SetText(TCollection_ExtendedString(qStr.toStdString().c_str(), true));

			auto applyGraphicsDefaults = [](const Handle(AIS_InteractiveObject)& obj) {
				obj->SetZLayer(Graphic3d_ZLayerId_Topmost);
				auto textLabel = Handle(AIS_TextLabel)::DownCast(obj);
				if (textLabel) {
					textLabel->SetDisplayType(Aspect_TODT_SUBTITLE);
					textLabel->SetColorSubTitle(Quantity_NOC_BLACK);
					textLabel->SetColor(Quantity_NOC_WHITE);
					textLabel->SetTransparency(0.2);
				}
				else {
					obj->SetColor(Quantity_NOC_BLACK);
				}
				};

			applyGraphicsDefaults(anAisLine);
			applyGraphicsDefaults(anAisTextLabel);
			applyGraphicsDefaults(anAisPnt1);
			applyGraphicsDefaults(anAisPnt2);

			m_vecMeasureFlags.emplace_back(anAisLine);
			m_vecMeasureFlags.emplace_back(anAisPnt1);
			m_vecMeasureFlags.emplace_back(anAisPnt2);
			m_vecMeasureFlags.emplace_back(anAisTextLabel);

			m_pViewer->Context()->Display(anAisLine, false);
			m_pViewer->Context()->Display(anAisPnt1, false);
			m_pViewer->Context()->Display(anAisPnt2, false);
			m_pViewer->Context()->Display(anAisTextLabel, false);
		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::measureAngle()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}

	auto vec = getCurSelected();

	if (vec.size() < 2)
	{
		return;
	}

	Mx3d::MeasureAngle ma;
	if (brepAngle(vec[0], vec[1], ma))
	{
		Mx3d::MeasureValue mv = ma;
		if (!std::holds_alternative<Mx3d::MeasureNone>(mv))
		{
			Handle(AIS_Line) anAisLine1 = new AIS_Line(new Geom_CartesianPoint(ma.pntCenter), new Geom_CartesianPoint(ma.pnt1));
			Handle(AIS_Line) anAisLine2 = new AIS_Line(new Geom_CartesianPoint(ma.pntCenter), new Geom_CartesianPoint(ma.pnt2));

			Handle(AIS_TextLabel) anAisTextLabel = new AIS_TextLabel();


			auto fnVecIsNull = [](const gp_Vec& v) {
				return v.SquareMagnitude() < Precision::SquareConfusion();
				};
			const gp_Vec vec1(ma.pntCenter, ma.pnt1);
			const gp_Vec vec2(ma.pntCenter, ma.pnt2);
			const gp_Dir nCircle = fnVecIsNull(vec1) || fnVecIsNull(vec2) ? gp::DZ() : gp_Dir{ vec1.Crossed(vec2) };
			const gp_Dir xCircle = fnVecIsNull(vec1) ? gp::DX() : gp_Dir{ vec1 };
			const gp_Ax2 axCircle(ma.pntCenter, nCircle, xCircle);
			Handle(Geom_Circle) geomCircle = new Geom_Circle(axCircle, 0.8 * vec1.Magnitude());
			const double param1 = ElCLib::Parameter(geomCircle->Circ(), ma.pnt1);
			const double param2 = ElCLib::Parameter(geomCircle->Circ(), ma.pnt2);
			Handle(AIS_Circle) anAisCircle = new AIS_Circle(geomCircle, param1, param2);

			auto applyGraphicsDefaults = [](const Handle(AIS_InteractiveObject)& obj) {
				obj->SetZLayer(Graphic3d_ZLayerId_Topmost);
				auto textLabel = Handle(AIS_TextLabel)::DownCast(obj);
				if (textLabel) {
					textLabel->SetDisplayType(Aspect_TODT_SUBTITLE);
					textLabel->SetColorSubTitle(Quantity_NOC_BLACK);
					textLabel->SetColor(Quantity_NOC_WHITE);
					textLabel->SetTransparency(0.2);
				}
				else {
					obj->SetColor(Quantity_NOC_BLACK);
				}
				};

			applyGraphicsDefaults(anAisLine1);
			applyGraphicsDefaults(anAisLine2);
			applyGraphicsDefaults(anAisTextLabel);
			applyGraphicsDefaults(anAisCircle);


			anAisCircle->SetWidth(2);
			anAisLine1->Attributes()->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT);
			anAisLine2->Attributes()->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT);
			anAisTextLabel->SetPosition(ElCLib::Value(param1 + std::abs((param2 - param1) / 2.), geomCircle->Circ()));
			QString qStr = QString(u8"%1°").arg(ma.value * 180 / 3.14159265358979323846);
			anAisTextLabel->SetText(TCollection_ExtendedString(qStr.toStdString().c_str(), true));

			m_vecMeasureFlags.emplace_back(anAisLine1);
			m_vecMeasureFlags.emplace_back(anAisLine2);
			m_vecMeasureFlags.emplace_back(anAisTextLabel);
			m_vecMeasureFlags.emplace_back(anAisCircle);

			m_pViewer->Context()->Display(anAisLine1, false);
			m_pViewer->Context()->Display(anAisLine2, false);
			m_pViewer->Context()->Display(anAisTextLabel, false);
			m_pViewer->Context()->Display(anAisCircle, false);

		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::measureLength()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}
	auto vec = getCurSelected();

	for (auto shape : vec) {
		if (shape.ShapeType() == TopAbs_EDGE)
		{
			Mx3d::MeasureValue mv;
			Mx3d::MeasureLength ml;

			if (brepLength(shape, ml))
			{
				mv = ml;
				if (!std::holds_alternative<Mx3d::MeasureNone>(mv))
				{
					Handle(AIS_TextLabel) anAisTextLabel = new AIS_TextLabel();
					anAisTextLabel->SetPosition(ml.middlePnt);
					QString qStr = QString(u8"%1").arg(ml.value);
					anAisTextLabel->SetText(TCollection_ExtendedString(qStr.toStdString().c_str(), true));

					anAisTextLabel->SetZLayer(Graphic3d_ZLayerId_Topmost);
					anAisTextLabel->SetDisplayType(Aspect_TODT_SUBTITLE);
					anAisTextLabel->SetColorSubTitle(Quantity_NOC_BLACK);
					anAisTextLabel->SetColor(Quantity_NOC_WHITE);
					anAisTextLabel->SetTransparency(0.2);

					m_vecMeasureFlags.emplace_back(anAisTextLabel);
					m_pViewer->Context()->Display(anAisTextLabel, false);
				}
			}


		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::measureArea()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}
	auto vec = getCurSelected();

	for (auto shape : vec) {
		if (shape.ShapeType() == TopAbs_FACE)
		{
			Mx3d::MeasureValue mv;
			Mx3d::MeasureArea ma;

			if (brepArea(shape, ma))
			{
				mv = ma;
				if (!std::holds_alternative<Mx3d::MeasureNone>(mv))
				{
					Handle(AIS_TextLabel) anAisTextLabel = new AIS_TextLabel();
					anAisTextLabel->SetPosition(ma.middlePnt);
					QString qStr = QString(u8"%1").arg(ma.value);
					anAisTextLabel->SetText(TCollection_ExtendedString(qStr.toStdString().c_str(), true));

					anAisTextLabel->SetZLayer(Graphic3d_ZLayerId_Topmost);
					anAisTextLabel->SetDisplayType(Aspect_TODT_SUBTITLE);
					anAisTextLabel->SetColorSubTitle(Quantity_NOC_BLACK);
					anAisTextLabel->SetColor(Quantity_NOC_WHITE);
					anAisTextLabel->SetTransparency(0.2);

					m_vecMeasureFlags.emplace_back(anAisTextLabel);
					m_pViewer->Context()->Display(anAisTextLabel, false);
				}
			}


		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::measureBoundingBox()
{
	for (auto f : m_vecMeasureFlags)
	{
		m_pViewer->Context()->Remove(f, false);
	}
	auto vec = getCurSelected();
	for (auto shape : vec) {
		TopAbs_ShapeEnum t = shape.ShapeType();

		if (true/*shape.ShapeType() == TopAbs_COMPOUND*/)
		{
			Mx3d::MeasureValue mv;
			Mx3d::MeasureBoundingBox mbb;

			if (brepBoundingBox(shape, mbb))
			{
				mv = mbb;
				if (!std::holds_alternative<Mx3d::MeasureNone>(mv))
				{
					Handle(AIS_Point) minPoint = new AIS_Point(new Geom_CartesianPoint(mbb.cornerMin));
					Handle(AIS_Point) maxPoint = new AIS_Point(new Geom_CartesianPoint(mbb.cornerMax));

					Handle(AIS_TextLabel) xLengthText = new AIS_TextLabel();
					Handle(AIS_TextLabel) yLengthText = new AIS_TextLabel();
					Handle(AIS_TextLabel) zLengthText = new AIS_TextLabel();

					Handle(AIS_InteractiveObject) anAisBox;

					const TopoDS_Shape shapeBox = BRepPrimAPI_MakeBox(mbb.cornerMin, mbb.cornerMax);
					auto aisShapeBox = new AIS_Shape(shapeBox);
					anAisBox = aisShapeBox;
					anAisBox->SetDisplayMode(AIS_Shaded);
					anAisBox->SetTransparency(0.85);
					anAisBox->Attributes()->SetFaceBoundaryDraw(true);
					anAisBox->Attributes()->SetFaceBoundaryAspect(new Prs3d_LineAspect({}, Aspect_TOL_DOT, 1.));

					const double diffX = mbb.cornerMax.X() - mbb.cornerMin.X();
					const double diffY = mbb.cornerMax.Y() - mbb.cornerMin.Y();
					const double diffZ = mbb.cornerMax.Z() - mbb.cornerMin.Z();
					xLengthText->SetPosition(mbb.cornerMin.Translated((diffX / 2.) * gp_Vec { 1, 0, 0 }));
					yLengthText->SetPosition(mbb.cornerMin.Translated((diffY / 2.) * gp_Vec { 0, 1, 0 }));
					zLengthText->SetPosition(mbb.cornerMin.Translated((diffZ / 2.) * gp_Vec { 0, 0, 1 }));

					xLengthText->SetZLayer(Graphic3d_ZLayerId_Topmost);
					xLengthText->SetDisplayType(Aspect_TODT_SUBTITLE);
					xLengthText->SetColorSubTitle(Quantity_NOC_BLACK);
					xLengthText->SetColor(Quantity_NOC_WHITE);
					xLengthText->SetTransparency(0.2);
					QString qStrX = QString(u8"%1").arg(mbb.xLength);
					xLengthText->SetText(TCollection_ExtendedString(qStrX.toStdString().c_str(), true));

					yLengthText->SetZLayer(Graphic3d_ZLayerId_Topmost);
					yLengthText->SetDisplayType(Aspect_TODT_SUBTITLE);
					yLengthText->SetColorSubTitle(Quantity_NOC_BLACK);
					yLengthText->SetColor(Quantity_NOC_WHITE);
					yLengthText->SetTransparency(0.2);
					QString qStrY = QString(u8"%1").arg(mbb.yLength);
					yLengthText->SetText(TCollection_ExtendedString(qStrY.toStdString().c_str(), true));

					zLengthText->SetZLayer(Graphic3d_ZLayerId_Topmost);
					zLengthText->SetDisplayType(Aspect_TODT_SUBTITLE);
					zLengthText->SetColorSubTitle(Quantity_NOC_BLACK);
					zLengthText->SetColor(Quantity_NOC_WHITE);
					zLengthText->SetTransparency(0.2);
					QString qStrZ = QString(u8"%1").arg(mbb.zLength);
					zLengthText->SetText(TCollection_ExtendedString(qStrZ.toStdString().c_str(), true));

					minPoint->SetZLayer(Graphic3d_ZLayerId_Topmost);
					minPoint->SetColor(Quantity_NOC_BLACK);

					maxPoint->SetZLayer(Graphic3d_ZLayerId_Topmost);
					maxPoint->SetColor(Quantity_NOC_BLACK);


					anAisBox->SetColor(Quantity_NOC_BEIGE);
					anAisBox->SetZLayer(Graphic3d_ZLayerId_Default);

					m_vecMeasureFlags.emplace_back(minPoint);
					m_vecMeasureFlags.emplace_back(maxPoint);
					m_vecMeasureFlags.emplace_back(xLengthText);
					m_vecMeasureFlags.emplace_back(yLengthText);
					m_vecMeasureFlags.emplace_back(zLengthText);
					m_vecMeasureFlags.emplace_back(anAisBox);

					m_pViewer->Context()->Display(minPoint, false);
					m_pViewer->Context()->Display(maxPoint, false);
					m_pViewer->Context()->Display(xLengthText, false);
					m_pViewer->Context()->Display(yLengthText, false);
					m_pViewer->Context()->Display(zLengthText, false);
					m_pViewer->Context()->Display(anAisBox, false);

				}
			}

		}
	}
	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::drawShapes()
{
	emit startRender();

	TDF_LabelSequence freeShapes;
	m_spShapeTool->GetFreeShapes(freeShapes);
	for (TDF_LabelSequence::Iterator aLabIter(freeShapes); aLabIter.More(); aLabIter.Next())
	{
		const TDF_Label& aLabel = aLabIter.Value();
		deepBuildAssemblyTree(0, aLabel);
	}
	for (Mx3d::TreeNodeId rootId : m_modelTree.roots())
	{
		mapEntity(rootId);
	}
	m_pSectionTools->setRange(m_modelBndBox);
	emit fileReady(true);
	emit stopProgress();

	// save recent file
	MxRecentFile MxRecentFile = createRecentFile();
	MxRecentFileManager::instance()->addRecentFile(MxRecentFile);


	m_pViewer->View()->FitAll();


	auto* modelTreeRoot = new QTreeWidgetItem(m_pModelTree);
	modelTreeRoot->setText(0, QFileInfo(m_filePath).fileName());
	for (Mx3d::TreeNodeId entityId : m_modelTree.roots())
	{
		buildModelTreeItems(modelTreeRoot, entityId);
	}
	m_pModelTree->expandToDepth(1);
}

MxRecentFile Mx3dGuiDocument::createRecentFile()
{
    Mx3d::Thumbnail thumbnail = m_pViewer->createThumbnail();
	MxRecentFile recentFile;
	QFileInfo fileInfo(m_filePath);
	recentFile.filePath = m_filePath;
	recentFile.lastOpened = QDateTime::currentDateTime();
	recentFile.fileType = fileInfo.suffix();
	recentFile.fileSize = fileInfo.size();
	recentFile.thumbnail = thumbnail.imageData;

	return recentFile;
}

void Mx3dGuiDocument::applyExplodingFactor(const GraphicsEntity& entity, double t)
{
	const gp_Pnt entityCenter = Mx3d::BndBoxCoords::get(entity.bndBox).center();
	for (const GraphicsEntity::Object& object : entity.vecObject) {
		const gp_Vec vecDirection(entityCenter, Mx3d::BndBoxCoords::get(object.bndBox).center());
		gp_Trsf trsfMove;
		trsfMove.SetTranslation(2 * t * vecDirection);
		m_pViewer->Context()->SetLocation(object.ptr, trsfMove * object.trsfOriginal);
	}
}
const Mx3dGuiDocument::GraphicsEntity* Mx3dGuiDocument::findGraphicsEntity(Mx3d::TreeNodeId entityTreeNodeId) const
{
	auto itFound = std::find_if(
		m_vecGraphicsEntity.cbegin(),
		m_vecGraphicsEntity.cend(),
		[=](const GraphicsEntity& item) { return item.treeNodeId == entityTreeNodeId; }
	);
	return itFound != m_vecGraphicsEntity.cend() ? &(*itFound) : nullptr;
}
void Mx3dGuiDocument::setSelect(Mx3d::TreeNodeId nodeId)
{
	m_pViewer->Context()->ClearSelected(false);
	const Mx3d::TreeNodeId entityNodeId = m_modelTree.nodeRoot(nodeId);
	const GraphicsEntity* gfxEntity = this->findGraphicsEntity(entityNodeId);
	if (!gfxEntity)
		return;

	traverseTree(nodeId, m_modelTree, [=](Mx3d::TreeNodeId id) {
		Handle(AIS_InteractiveObject) gfxObject = Mx3d::CppUtils::findValue(id, gfxEntity->mapTreeNodeGfxObject);
		if (gfxObject)
		{
			auto gfxOwner = gfxObject->GlobalSelOwner();
			auto gfxObject = Handle(AIS_InteractiveObject)::DownCast(
				gfxOwner ? gfxOwner->Selectable() : Handle(SelectMgr_SelectableObject)()
			);
			if (m_pViewer->Context()->IsDisplayed(gfxObject))
				m_pViewer->Context()->AddOrRemoveSelected(gfxOwner, false);
		}
		});
	m_pViewer->View()->Redraw();
	m_pViewer->update();
	return;

}

void Mx3dGuiDocument::setProperty(Mx3d::TreeNodeId nodeId)
{
	m_pPropertyTable->clear();

	TDF_Label label = m_modelTree.nodeData(nodeId);

	Mx3d::Properties attrs = Mx3d::getLabelProperties(label);

	for (auto attr : attrs)
	{
		if (!attr.second.isEmpty())
		{
			QTreeWidgetItem* item1 = new QTreeWidgetItem(m_pPropertyTable);
			item1->setText(0, attr.first);
			item1->setText(1, attr.second);
			if (attr.first == tr("Color") || attr.first == tr("Product Color"))
			{
				QColor color;
				if (attr.second.startsWith("(") && attr.second.endsWith(")"))
				{
					QStringList strList = attr.second.mid(1, attr.second.length() - 2).split(",");
					if (strList.size() == 3)
					{
						bool ok1, ok2, ok3;
						double r = strList[0].trimmed().toDouble(&ok1);
						double g = strList[1].trimmed().toDouble(&ok2);
						double b = strList[2].trimmed().toDouble(&ok3);
						if (ok1 && ok2 && ok3)
						{
							color.setRgbF(r, g, b);
						}
					}
				}
				if (!color.isValid())
				{
					color = QColor(Qt::transparent);
				}

				QPixmap pixmap(40, 40);
				pixmap.fill(Qt::transparent);
				QPainter painter(&pixmap);
				painter.setPen(QPen(Qt::black, 2));
				painter.setBrush(QBrush(color));
				painter.drawRect(0, 0, 40, 40);
				painter.end();
				item1->setIcon(1, QIcon(pixmap));
			}
		}
	}
}


void Mx3dGuiDocument::openModelTree()
{
	if (m_pLeftSplitter->isHidden())
	{
		m_pLeftSplitter->show();
	}
	else
	{
		m_pLeftSplitter->hide();
	}
}

void Mx3dGuiDocument::explodeModel(int value)
{
	double dValue = value / 100.0;
	if (dValue > 100.0)
	{
		dValue = 100.0;
	}
	else if (dValue < 0.0)
	{
		dValue = 0.0;
	}
	m_explodingFactor = dValue;
	for (const GraphicsEntity& entity : m_vecGraphicsEntity)
		applyExplodingFactor(entity, dValue);

	m_pViewer->View()->Redraw();
}

void Mx3dGuiDocument::onModelTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (current)
	{
		Mx3d::TreeNodeId id = 0;
		bool ok = false;
		id = current->data(0, Qt::UserRole + 3).toInt(&ok);
		if (!ok) {
			return;
		}
		setSelect(id);
		setProperty(id);
	}
	else {
		m_pViewer->Context()->ClearSelected(true);
	}
}


void Mx3dGuiDocument::addClipPlanes(std::vector<Handle_Graphic3d_ClipPlane> clipPlanes)
{
	for (auto clipPlane : clipPlanes)
	{
		if (!Mx3d::hasClipPlane(m_pViewer->View(), clipPlane))
		{
			m_pViewer->View()->AddClipPlane(clipPlane);
		}
	}
}

Mx3dProgressIndicator * Mx3dGuiDocument::getProgressIndicator()
{
	return m_pProgressIndicator;
}

void Mx3dGuiDocument::openFile(const QString& filePath)
{
	MxUtils::MxFormat format = MxUtils::MxFormat::Format_Unknown;
	format = MxUtils::getFileFormat(filePath);
	if (MxUtils::MxFormat::Format_Unknown == format)
	{
		emit fileReady(false);
		QMessageBox::warning(this, tr("Open File"), tr("Unsupported file format"));
		return;
	}
	
	QtConcurrent::run([this, filePath, format]()
		{
			openFile(filePath, format);
		});
}

void Mx3dGuiDocument::openFile(const QString& filePath, MxUtils::MxFormat format)
{
	emit startReadFile(filePath);
	bool isOpenSuccess = false;
	switch (format)
	{
	case MxUtils::MxFormat::Format_STEP:
		isOpenSuccess = openStepFile(filePath);
		break;
	case MxUtils::MxFormat::Format_IGES:
		isOpenSuccess = openIgesFile(filePath);
		break;
	case MxUtils::MxFormat::Format_STL:
		isOpenSuccess = openStlFile(filePath);
		break;
	case MxUtils::MxFormat::Format_VRML:
		isOpenSuccess = openVrmlFile(filePath);
		break;
	case MxUtils::MxFormat::Format_OCCBREP:
		isOpenSuccess = openOccBrepFile(filePath);
		break;
	case MxUtils::MxFormat::Format_GLTF:
		isOpenSuccess = openGltfFile(filePath);
		break;
	case MxUtils::MxFormat::Format_OBJ:
		isOpenSuccess = openObjFile(filePath);
		break;
	default:
		break;
	}
	if (isOpenSuccess)
	{
		m_filePath = filePath;
		emit fileOpened();
		return;
	}
	emit fileReady(false);
	emit stopProgress();
}

bool Mx3dGuiDocument::openStepFile(const QString& filePath)
{
	STEPCAFControl_Reader aStepReader;
	const IFSelect_ReturnStatus stat = aStepReader.ReadFile(filePath.toStdString().c_str());

	if (IFSelect_RetDone != stat)
	{
		return false;
	}
	const bool transferStat = aStepReader.Transfer(m_spDocument, m_pProgressIndicator->Start());
	if (!transferStat)
	{
		return false;
	}
	return true;
}

bool Mx3dGuiDocument::openIgesFile(const QString& filePath)
{
	IGESCAFControl_Reader aIgesReader;
	aIgesReader.SetColorMode(true);
	aIgesReader.SetNameMode(true);
	aIgesReader.SetLayerMode(true);


	const IFSelect_ReturnStatus stat = aIgesReader.ReadFile(filePath.toStdString().c_str());

	if (IFSelect_RetDone != stat)
	{
		return false;
	}

	const bool transferStat = aIgesReader.Transfer(m_spDocument, m_pProgressIndicator->Start());

	if (!transferStat)
	{
		return false;
	}
	return true;
}

bool Mx3dGuiDocument::openStlFile(const QString& filePath)
{
	Handle(Poly_Triangulation) mesh = RWStl::ReadFile(filePath.toStdString().c_str(), 0.62831852); // PI/5

	if (mesh.IsNull())
	{
		return false;
	}

	TopoDS_Face face;
	BRep_Builder builder;
	builder.MakeFace(face);
	builder.UpdateFace(face, mesh);
	TDF_Label label = m_spShapeTool->AddShape(face, Standard_False, Standard_False);
	TDataStd_Name::Set(label, "MESH STL FACE");
	m_spShapeTool->UpdateAssemblies();

	return true;
}

bool Mx3dGuiDocument::openVrmlFile(const QString& filePath)
{

	VrmlAPI_CafReader aVrmlReader;
	aVrmlReader.SetSystemLengthUnit(1.0);
	aVrmlReader.SetSystemCoordinateSystem(RWMesh_CoordinateSystem_Zup);
	aVrmlReader.SetDocument(m_spDocument);

	const bool stat = aVrmlReader.Perform(filePath.toStdString().c_str(), m_pProgressIndicator->Start());

	if (!stat)
	{
		return false;
	}

	return true;
}

bool Mx3dGuiDocument::openOccBrepFile(const QString& filePath)
{
	TopoDS_Shape aShape;
	BRep_Builder brepBuilder;
	const bool stat = BRepTools::Read(aShape, filePath.toStdString().c_str(), brepBuilder, m_pProgressIndicator->Start());

	if (!stat || aShape.IsNull())
	{
		return false;
	}

	m_spShapeTool->AddShape(aShape, Standard_False, Standard_False);
	m_spShapeTool->UpdateAssemblies();

	return true;
}

bool Mx3dGuiDocument::openGltfFile(const QString& filePath)
{
	RWGltf_CafReader aGltfReader;
	aGltfReader.SetSystemLengthUnit(-1);
	aGltfReader.SetSystemCoordinateSystem(RWMesh_CoordinateSystem_Undefined);
	aGltfReader.SetSkipEmptyNodes(true);
	aGltfReader.SetMeshNameAsFallback(true);
	aGltfReader.SetDocument(m_spDocument);

	const bool stat = aGltfReader.Perform(filePath.toStdString().c_str(), m_pProgressIndicator->Start());

	if (!stat)
	{
		return false;
	}

	return true;
}

bool Mx3dGuiDocument::openObjFile(const QString& filePath)
{
	RWObj_CafReader anObjReader;
	anObjReader.SetSystemLengthUnit(-1);
	anObjReader.SetSystemCoordinateSystem(RWMesh_CoordinateSystem_Undefined);
	anObjReader.SetSinglePrecision(true);
	anObjReader.SetDocument(m_spDocument);

	const bool stat = anObjReader.Perform(filePath.toStdString().c_str(), m_pProgressIndicator->Start());

	if (!stat)
	{
		return false;
	}

	return true;
}
