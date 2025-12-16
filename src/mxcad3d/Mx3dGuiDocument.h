/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QWidget>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <AIS_InteractiveObject.hxx>
#include <QString>

#include "MxUtils.h"
#include "MxRecentFile.h"
#include "Mx3dTree.h"
#include "Mx3dLabelHasher.h"
#include "Mx3dHandleHasher.h"
#include "Mx3dMeasureType.h"

class Mx3dViewer;
class QSplitter;
class MxTreeWidget;
class MxViewToolBar;
class Mx3dProgressIndicator;
class Mx3dExplodeSlider;
class Mx3dSectionTools;
class QTreeWidgetItem;
class QActionGroup;

class Mx3dGuiDocument : public QWidget
{
	Q_OBJECT
public:
	Mx3dGuiDocument(QWidget* parent = nullptr);
	~Mx3dGuiDocument();

	
	void addClipPlanes(std::vector<Handle_Graphic3d_ClipPlane> clipPlanes);
	Mx3dProgressIndicator* getProgressIndicator();
public:
	void openFile(const QString& filePath);
private:
	void openFile(const QString& filePath, MxUtils::MxFormat format);

	bool openStepFile(const QString& filePath);
	bool openIgesFile(const QString& filePath);
	bool openStlFile(const QString& filePath);
	bool openVrmlFile(const QString& filePath);
	bool openOccBrepFile(const QString& filePath);
	bool openGltfFile(const QString& filePath);
	bool openObjFile(const QString& filePath);
private:

	struct GraphicsEntity {
		struct Object {
			Object(const Handle(AIS_InteractiveObject)& p) : ptr(p) {}
			Handle(AIS_InteractiveObject) ptr;
			gp_Trsf trsfOriginal;
			Bnd_Box bndBox;
		};

		Mx3d::TreeNodeId treeNodeId;
		std::vector<Object> vecObject;
		std::unordered_map<Mx3d::TreeNodeId, Handle(AIS_InteractiveObject)> mapTreeNodeGfxObject;
		std::unordered_map<Handle(AIS_InteractiveObject), Mx3d::TreeNodeId> mapGfxObjectTreeNode;
		Bnd_Box bndBox;
	};
	std::vector<GraphicsEntity> m_vecGraphicsEntity;


	const GraphicsEntity* findGraphicsEntity(Mx3d::TreeNodeId entityTreeNodeId) const;

	enum class CheckState { Off, Partially, On };
	std::unordered_map<Mx3d::TreeNodeId, CheckState> m_mapTreeNodeCheckState;
	std::unordered_map<Mx3d::TreeNodeId, QTreeWidgetItem*> m_mapNodeIdToTreeItem;

private:
	void setupUi();
	void createViewToolBar();
	void initDocument();
	void connectSignals();


	TopLoc_Location shapeReferenceLocation(const TDF_Label& lbl);
	TopLoc_Location shapeAbsoluteLocation(const Mx3d::Tree<TDF_Label>& modelTree, Mx3d::TreeNodeId nodeId);
	Bnd_Box aisObjectBoundingBox(const Handle(AIS_InteractiveObject)& object);
	void addBndBoxToEntity(Bnd_Box* box, const Bnd_Box& other);
	Handle(AIS_InteractiveObject) createGraphicsObject(const TDF_Label& label);
	Mx3d::TreeNodeId deepBuildAssemblyTree(Mx3d::TreeNodeId parentNode, const TDF_Label& label);
	void mapEntity(Mx3d::TreeNodeId entityTreeNodeId);

	Mx3d::TreeNodeId nodeFromGraphicsObject(const Handle(AIS_InteractiveObject)& gfxObject) const;

	const TCollection_ExtendedString& labelAttrStdName(const TDF_Label& label);
	QString referenceItemText(const TDF_Label& instanceLabel, const TDF_Label& productLabel);
	QIcon shapeIcon(const TDF_Label& label);
	QTreeWidgetItem* buildModelTreeItems(QTreeWidgetItem* treeItem, const Mx3d::TreeNodeId& id);

	std::vector<TopAbs_ShapeEnum> selectionModes(Mx3d::MeasureType type);
	void stopMeasure();
	void enableSelectionMode(Mx3d::MeasureType type);
	
	std::vector<TopoDS_Shape> getCurSelected();
	void measureVertex();
	void measureCircleCenter();
	void measureCircleDiameter();
	void measureMinDistance();
	void measureCenterDistance();
	void measureAngle();
	void measureLength();
	void measureArea();
	void measureBoundingBox();

	void drawShapes();

	MxRecentFile createRecentFile();


	void applyExplodingFactor(const GraphicsEntity& entity, double t);

	void setSelect(Mx3d::TreeNodeId nodeId);
	void setProperty(Mx3d::TreeNodeId nodeId);
private:
	void openModelTree();
	void explodeModel(int value);
	void onModelTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
private: // document
	Handle(TDocStd_Document)		m_spDocument;
	Handle(XCAFDoc_ShapeTool)		m_spShapeTool;
	Handle(XCAFDoc_ColorTool)		m_spColorTool;
	Handle(XCAFDoc_LayerTool)		m_spLayerTool;
	Mx3dProgressIndicator*			m_pProgressIndicator = nullptr;
	QString							m_filePath;
	Mx3d::Tree<TDF_Label>			m_modelTree;
	Bnd_Box							m_modelBndBox;
	double  						m_explodingFactor;
	bool 							m_isMeasuring = false;
	Mx3d::MeasureType				m_measureType = Mx3d::MeasureType::None;
	std::vector<Handle(AIS_InteractiveObject)> m_vecMeasureFlags;
private: // ui
	Mx3dViewer* m_pViewer = nullptr;
	QSplitter* m_pMainSplitter = nullptr;
	QSplitter* m_pLeftSplitter = nullptr;
    MxTreeWidget* m_pModelTree = nullptr;
	MxTreeWidget* m_pPropertyTable = nullptr;
    MxViewToolBar* m_pViewToolBar = nullptr;
	Mx3dExplodeSlider* m_pExplodeSlider = nullptr;
	MxViewToolBar* m_pMeasureToolBar = nullptr;
	Mx3dSectionTools* m_pSectionTools = nullptr;
	QActionGroup* m_pActionGroup_1 = nullptr;
	QActionGroup* m_pActionGroup_2 = nullptr;
signals:
	void startReadFile(const QString& filePath);
	void startRender();
	void stopProgress();

	void fileOpened();
	void fileReady(bool success);
};

