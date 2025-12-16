/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx3dSectionTools.h"
#include <QPainter>
#include <QFile>
#include <Image_AlienPixMap.hxx>
#include "Mx3dUtils.h"
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>

Mx3dSectionTools::Mx3dSectionTools(QWidget* parent)
	: QWidget(parent)
{
	initUi();

	createPlaneCappingTexture();
	initClipPlanes();

	setCursor(Qt::ArrowCursor);

	connectSignals();
}

Mx3dSectionTools::~Mx3dSectionTools()
{
}

std::vector<Handle_Graphic3d_ClipPlane> Mx3dSectionTools::getClipPlanes()
{
	return m_vecClipPlaneData;
}

void Mx3dSectionTools::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	QPainter painter(this);
	painter.fillRect(rect(), QColor(92, 92, 92, 100));
	painter.end();
}

void Mx3dSectionTools::initUi()
{
	if (objectName().isEmpty())
		setObjectName("Mx3dSectionToolsClass");
	resize(450, 206);
	setWindowTitle(tr("Mx3dSectionTools"));

	QString checkBoxStyle = R"(
		QCheckBox {
			padding: 10px 3px;
			color: white;
			font-size: 16px;
			font-weight: bold;
		}
		QCheckBox::indicator {
			width: 20px;
			height: 20px;
			background-color: white;
			border: 1px solid #888;
			border-radius: 3px;
		}
		QCheckBox::indicator:checked {
			image: url(:/resources/images3d/checked.svg);
		}
	)";

	QString sliderStyle = R"(
		/* Basic Theme */
		QSlider {
			padding: 10px 0;
		}
		
		/* Groove Style */
		QSlider::groove:horizontal {
			border: 1px solid #cccccc;
			background: #f0f0f0;
			height: 8px;
			border-radius: 4px;
		}
		
		/* Sub-page (filled) Style */
		QSlider::sub-page:horizontal {
			background: rgb(56, 142, 138);
			border: 1px solid rgb(46, 122, 118);
			height: 8px;
			border-radius: 4px;
		}
		
		/* Add-page (empty) Style */
		QSlider::add-page:horizontal {
			background: #f0f0f0;
			border: 1px solid #cccccc;
			height: 8px;
			border-radius: 4px;
		}
		
		/* Handle Style */
		QSlider::handle:horizontal {
			background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
									   stop:0 rgb(76, 162, 158), 
									   stop:1 rgb(56, 142, 138));
			border: 1px solid rgb(46, 122, 118);
			width: 20px;
			margin: -6px 0;
			border-radius: 10px;
			box-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);
		}
		
		/* Handle Hover Style */
		QSlider::handle:horizontal:hover {
			background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
									   stop:0 rgb(86, 172, 168), 
									   stop:1 rgb(66, 152, 148));
			border: 1px solid rgb(36, 112, 108);
			width: 22px;
			margin: -7px 0;
			box-shadow: 0 1px 4px rgba(0, 0, 0, 0.3);
		}
		
		/* Handle Pressed Style */
		QSlider::handle:horizontal:pressed {
			background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
									   stop:0 rgb(56, 142, 138), 
									   stop:1 rgb(36, 122, 118));
			border: 1px solid rgb(26, 102, 98);
			width: 24px;
			margin: -8px 0;
			box-shadow: 0 2px 5px rgba(0, 0, 0, 0.4);
		}
		
		/* Tickmark Style */
		QSlider::tickmark:horizontal {
			background: #bbbbbb;
			width: 1px;
			height: 6px;
			margin: 8px 0;
		}
	)";

	QString btnStyle = R"(
		QPushButton {
			border: 1px solid rgb( 56,142,138); 
			background-color: rgb( 56,142,138); 
			color: white; 
			border-radius: 2px;
			font-size: 16px;
			padding: 5px;
			font-weight: bold;
		}
		QPushButton:hover {
			background-color: rgba( 56,142,138, 0.8); 
		}
		QPushButton:pressed {
			background-color: rgb( 56,142,138); 
		}
	)";

	gridLayout_2 = new QGridLayout(this);
	gridLayout_2->setSpacing(6);
	gridLayout_2->setContentsMargins(5, 5, 5, 5);
	gridLayout_2->setHorizontalSpacing(5);
	gridLayout_2->setVerticalSpacing(0);

	// --- X Axis ---
	checkBox_xAxis = new QCheckBox(this);
	checkBox_xAxis->setObjectName("checkBox_xAxis");
	checkBox_xAxis->setMinimumSize(QSize(0, 0));
	checkBox_xAxis->setStyleSheet(checkBoxStyle);
	checkBox_xAxis->setText(tr("X Plane"));
	gridLayout_2->addWidget(checkBox_xAxis, 0, 0, 1, 1);

	widget_X = new QWidget(this);
	widget_X->setObjectName("widget_X");
	horizontalLayout = new QHBoxLayout(widget_X);
	horizontalLayout->setSpacing(6);
	horizontalLayout->setContentsMargins(0, 0, 0, 0);

	doubleSpinBox_xAxis = new QDoubleSpinBox(widget_X);
	doubleSpinBox_xAxis->setObjectName("doubleSpinBox_xAxis");
	horizontalLayout->addWidget(doubleSpinBox_xAxis);

	horizontalSlider_xAxis = new QSlider(widget_X);
	horizontalSlider_xAxis->setObjectName("horizontalSlider_xAxis");
	horizontalSlider_xAxis->setMinimumSize(QSize(84, 0));
	horizontalSlider_xAxis->setStyleSheet(sliderStyle);
	horizontalSlider_xAxis->setOrientation(Qt::Horizontal);
	horizontalLayout->addWidget(horizontalSlider_xAxis);

	btn_xAxis = new QPushButton(widget_X);
	btn_xAxis->setObjectName("btn_xAxis");
	btn_xAxis->setStyleSheet(btnStyle);
	btn_xAxis->setText(tr("Invert"));
	horizontalLayout->addWidget(btn_xAxis);

	gridLayout_2->addWidget(widget_X, 0, 1, 1, 1);
	widget_X->setEnabled(false);
	// --- Y Axis ---
	checkBox_yAxis = new QCheckBox(this);
	checkBox_yAxis->setObjectName("checkBox_yAxis");
	checkBox_yAxis->setMinimumSize(QSize(0, 0));
	checkBox_yAxis->setStyleSheet(checkBoxStyle);
	checkBox_yAxis->setIconSize(QSize(16, 16));
	checkBox_yAxis->setText(tr("Y Plane"));
	gridLayout_2->addWidget(checkBox_yAxis, 1, 0, 1, 1);

	widget_Y = new QWidget(this);
	widget_Y->setObjectName("widget_Y");
	horizontalLayout_2 = new QHBoxLayout(widget_Y);
	horizontalLayout_2->setSpacing(6);
	horizontalLayout_2->setContentsMargins(0, 0, 0, 0);

	doubleSpinBox_yAxis = new QDoubleSpinBox(widget_Y);
	doubleSpinBox_yAxis->setObjectName("doubleSpinBox_yAxis");
	horizontalLayout_2->addWidget(doubleSpinBox_yAxis);

	horizontalSlider_yAxis = new QSlider(widget_Y);
	horizontalSlider_yAxis->setObjectName("horizontalSlider_yAxis");
	horizontalSlider_yAxis->setMinimumSize(QSize(84, 0));
	horizontalSlider_yAxis->setStyleSheet(sliderStyle);
	horizontalSlider_yAxis->setOrientation(Qt::Horizontal);
	horizontalLayout_2->addWidget(horizontalSlider_yAxis);

	btn_yAxis = new QPushButton(widget_Y);
	btn_yAxis->setObjectName("btn_yAxis");
	btn_yAxis->setStyleSheet(btnStyle);
	btn_yAxis->setText(tr("Invert"));
	horizontalLayout_2->addWidget(btn_yAxis);

	gridLayout_2->addWidget(widget_Y, 1, 1, 1, 1);
	widget_Y->setEnabled(false);
	// --- Z Axis ---
	checkBox_zAxis = new QCheckBox(this);
	checkBox_zAxis->setObjectName("checkBox_zAxis");
	checkBox_zAxis->setMinimumSize(QSize(0, 0));
	checkBox_zAxis->setStyleSheet(checkBoxStyle);
	checkBox_zAxis->setText(tr("Z Plane"));
	gridLayout_2->addWidget(checkBox_zAxis, 2, 0, 1, 1);

	widget_Z = new QWidget(this);
	widget_Z->setObjectName("widget_Z");
	horizontalLayout_3 = new QHBoxLayout(widget_Z);
	horizontalLayout_3->setSpacing(6);
	horizontalLayout_3->setContentsMargins(0, 0, 0, 0);

	doubleSpinBox_zAxis = new QDoubleSpinBox(widget_Z);
	doubleSpinBox_zAxis->setObjectName("doubleSpinBox_zAxis");
	horizontalLayout_3->addWidget(doubleSpinBox_zAxis);

	horizontalSlider_zAxis = new QSlider(widget_Z);
	horizontalSlider_zAxis->setObjectName("horizontalSlider_zAxis");
	horizontalSlider_zAxis->setMinimumSize(QSize(84, 0));
	horizontalSlider_zAxis->setStyleSheet(sliderStyle);
	horizontalSlider_zAxis->setOrientation(Qt::Horizontal);
	horizontalLayout_3->addWidget(horizontalSlider_zAxis);

	btn_zAxis = new QPushButton(widget_Z);
	btn_zAxis->setObjectName("btn_zAxis");
	btn_zAxis->setStyleSheet(btnStyle);
	btn_zAxis->setText(tr("Invert"));
	horizontalLayout_3->addWidget(btn_zAxis);

	gridLayout_2->addWidget(widget_Z, 2, 1, 1, 1);
	widget_Z->setEnabled(false);
	// --- Custom ---
	checkBox_custom = new QCheckBox(this);
	checkBox_custom->setObjectName("checkBox_custom");
	checkBox_custom->setStyleSheet(checkBoxStyle);
	checkBox_custom->setText(tr("Custom"));
	gridLayout_2->addWidget(checkBox_custom, 3, 0, 1, 1);

	widget_Custom = new QWidget(this);
	widget_Custom->setObjectName("widget_Custom");
	gridLayout = new QGridLayout(widget_Custom);
	gridLayout->setSpacing(6);
	gridLayout->setContentsMargins(0, 3, 0, 3);
	gridLayout->setHorizontalSpacing(6);
	gridLayout->setVerticalSpacing(0);

	doubleSpinBox_custom = new QDoubleSpinBox(widget_Custom);
	doubleSpinBox_custom->setObjectName("doubleSpinBox_custom");
	gridLayout->addWidget(doubleSpinBox_custom, 0, 0, 1, 1);

	horizontalSlider_custom = new QSlider(widget_Custom);
	horizontalSlider_custom->setObjectName("horizontalSlider_custom");
	horizontalSlider_custom->setStyleSheet(sliderStyle);
	horizontalSlider_custom->setOrientation(Qt::Horizontal);
	gridLayout->addWidget(horizontalSlider_custom, 0, 1, 1, 1);

	btn_custom = new QPushButton(widget_Custom);
	btn_custom->setObjectName("btn_custom");
	btn_custom->setStyleSheet(btnStyle);
	btn_custom->setText(tr("Invert"));
	gridLayout->addWidget(btn_custom, 0, 2, 1, 1);

	horizontalLayout_4 = new QHBoxLayout();
	horizontalLayout_4->setSpacing(6);
	horizontalLayout_4->setObjectName("horizontalLayout_4");

	doubleSpinBox_custom_xDir = new QDoubleSpinBox(widget_Custom);
	doubleSpinBox_custom_xDir->setObjectName("doubleSpinBox_custom_xDir");
	doubleSpinBox_custom_xDir->setSingleStep(0.1);
	doubleSpinBox_custom_xDir->setPrefix(tr("X "));
	horizontalLayout_4->addWidget(doubleSpinBox_custom_xDir);

	doubleSpinBox_custom_yDir = new QDoubleSpinBox(widget_Custom);
	doubleSpinBox_custom_yDir->setObjectName("doubleSpinBox_custom_yDir");
	doubleSpinBox_custom_yDir->setSingleStep(0.1);
	doubleSpinBox_custom_yDir->setPrefix(tr("Y "));
	horizontalLayout_4->addWidget(doubleSpinBox_custom_yDir);

	doubleSpinBox_custom_zDir = new QDoubleSpinBox(widget_Custom);
	doubleSpinBox_custom_zDir->setObjectName("doubleSpinBox_custom_zDir");
	doubleSpinBox_custom_zDir->setSingleStep(0.1);
	doubleSpinBox_custom_zDir->setPrefix(tr("Z "));
	horizontalLayout_4->addWidget(doubleSpinBox_custom_zDir);

	gridLayout->addLayout(horizontalLayout_4, 1, 0, 1, 3);

	gridLayout_2->addWidget(widget_Custom, 3, 1, 1, 1);
    widget_Custom->setEnabled(false);
}

void Mx3dSectionTools::connectSignals()
{
	connectSignalsX();
	connectSignalsY();
	connectSignalsZ();
	connectSignalsCustom();
}

void Mx3dSectionTools::connectSignalsX()
{
	connect(checkBox_xAxis, &QCheckBox::toggled, this, &Mx3dSectionTools::setXEnabled);
	connect(horizontalSlider_xAxis, &QSlider::valueChanged, this, &Mx3dSectionTools::setXClipPlanePosition);
	connect(horizontalSlider_xAxis, &QSlider::valueChanged, this, [this](int value) {
		doubleSpinBox_xAxis->setValue(static_cast<double>(value));
		});
	connect(doubleSpinBox_xAxis, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, [this](double value) {
			horizontalSlider_xAxis->setValue(static_cast<int>(value));
		});
	connect(btn_xAxis, &QPushButton::clicked, this, [this]() {
		const gp_Dir invNormal = m_clipPlaneX->ToPlane().Axis().Direction().Reversed();
		setXClipPlaneNormal(invNormal);
		setXClipPlanePosition(doubleSpinBox_xAxis->value());
		});
}

void Mx3dSectionTools::connectSignalsY()
{
	connect(checkBox_yAxis, &QCheckBox::toggled, this, &Mx3dSectionTools::setYEnabled);
	connect(horizontalSlider_yAxis, &QSlider::valueChanged, this, &Mx3dSectionTools::setYClipPlanePosition);
	connect(horizontalSlider_yAxis, &QSlider::valueChanged, this, [this](int value) {
		doubleSpinBox_yAxis->setValue(static_cast<double>(value));
		});
	connect(doubleSpinBox_yAxis, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, [this](double value) {
			horizontalSlider_yAxis->setValue(static_cast<int>(value));
		});
	connect(btn_yAxis, &QPushButton::clicked, this, [this]() {
		const gp_Dir invNormal = m_clipPlaneY->ToPlane().Axis().Direction().Reversed();
		setYClipPlaneNormal(invNormal);
		setYClipPlanePosition(doubleSpinBox_yAxis->value());
		});
}

void Mx3dSectionTools::connectSignalsZ()
{
	connect(checkBox_zAxis, &QCheckBox::toggled, this, &Mx3dSectionTools::setZEnabled);
	connect(horizontalSlider_zAxis, &QSlider::valueChanged, this, &Mx3dSectionTools::setZClipPlanePosition);
	connect(horizontalSlider_zAxis, &QSlider::valueChanged, this, [this](int value) {
		doubleSpinBox_zAxis->setValue(static_cast<double>(value));
		});
	connect(doubleSpinBox_zAxis, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, [this](double value) {
			horizontalSlider_zAxis->setValue(static_cast<int>(value));
		});
	connect(btn_zAxis, &QPushButton::clicked, this, [this]() {
		const gp_Dir invNormal = m_clipPlaneZ->ToPlane().Axis().Direction().Reversed();
		setZClipPlaneNormal(invNormal);
		setZClipPlanePosition(doubleSpinBox_zAxis->value());
		});
}

void Mx3dSectionTools::connectSignalsCustom()
{
	connect(checkBox_custom, &QCheckBox::toggled, this, &Mx3dSectionTools::setCustomEnabled);
	connect(horizontalSlider_custom, &QSlider::valueChanged, this, &Mx3dSectionTools::setCustomClipPlanePosition);
	connect(horizontalSlider_custom, &QSlider::valueChanged, this, [this](int value) {
		doubleSpinBox_custom->setValue(static_cast<double>(value));
		});
	connect(doubleSpinBox_custom, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, [this](double value) {
			horizontalSlider_custom->setValue(static_cast<int>(value));
		});
	connect(btn_custom, &QPushButton::clicked, this, [this]() {
		const gp_Dir invNormal = m_clipPlaneCustom->ToPlane().Axis().Direction().Reversed();
		setCustomClipPlaneNormal(invNormal);
		setCustomClipPlanePosition(doubleSpinBox_custom->value());
		});
	connect(doubleSpinBox_custom_xDir, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		gp_Dir dir;
		if (getCustomDirection(dir))
		{
			setCustomClipPlaneDirection(dir);
		}
		});
	connect(doubleSpinBox_custom_yDir, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		gp_Dir dir;
		if (getCustomDirection(dir))
		{
			setCustomClipPlaneDirection(dir);
		}
		});
	connect(doubleSpinBox_custom_zDir, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		gp_Dir dir;
		if (getCustomDirection(dir))
		{
			setCustomClipPlaneDirection(dir);
		}
		});
}

bool Mx3dSectionTools::getCustomDirection(gp_Dir& dir)
{
	try
	{
		dir = gp_Dir(doubleSpinBox_custom_xDir->value(), doubleSpinBox_custom_yDir->value(), doubleSpinBox_custom_zDir->value());
	}
	catch (Standard_ConstructionError&)
	{
		return false;
	}
	return true;
}


void Mx3dSectionTools::setXEnabled(bool isEnabled)
{
	widget_X->setEnabled(isEnabled);
	m_clipPlaneX->SetOn(isEnabled);
	emit sectionToolsChanged();
}

void Mx3dSectionTools::setYEnabled(bool isEnabled)
{
	widget_Y->setEnabled(isEnabled);
	m_clipPlaneY->SetOn(isEnabled);
	emit sectionToolsChanged();
}

void Mx3dSectionTools::setZEnabled(bool isEnabled)
{
	widget_Z->setEnabled(isEnabled);
	m_clipPlaneZ->SetOn(isEnabled);
	emit sectionToolsChanged();
}

void Mx3dSectionTools::setCustomEnabled(bool isEnabled)
{
	widget_Custom->setEnabled(isEnabled);
	m_clipPlaneCustom->SetOn(isEnabled);
	emit sectionToolsChanged();
}


void Mx3dSectionTools::initClipPlanes()
{
	m_clipPlaneX = new Graphic3d_ClipPlane(gp_Pln(gp::Origin(), gp::DX()));
	m_clipPlaneY = new Graphic3d_ClipPlane(gp_Pln(gp::Origin(), gp::DY()));
	m_clipPlaneZ = new Graphic3d_ClipPlane(gp_Pln(gp::Origin(), gp::DZ()));
	m_clipPlaneCustom = new Graphic3d_ClipPlane(gp_Pln(gp::Origin(), gp_Dir(1, 1, 1)));


	m_vecClipPlaneData = {
		m_clipPlaneX,
		m_clipPlaneY,
		m_clipPlaneZ,
		m_clipPlaneCustom
	};

	for (auto& plane : m_vecClipPlaneData)
	{
		plane->SetCapping(true);
		plane->SetUseObjectMaterial(true);
		plane->SetCappingTexture(m_textureCapping);
		plane->SetOn(false);
	}
}


void Mx3dSectionTools::createPlaneCappingTexture()
{
	if (!m_textureCapping.IsNull())
		return;
	QFile file(":/resources/images3d/opencascade_hatch_1.png");
	if (file.open(QIODevice::ReadOnly)) {
		const QByteArray fileContents = file.readAll();
		const QByteArray filenameUtf8 = file.fileName().toUtf8();
		auto fileContentsData = reinterpret_cast<const Standard_Byte*>(fileContents.constData());
		Handle_Image_AlienPixMap imageCapping = new Image_AlienPixMap();
		imageCapping->Load(fileContentsData, fileContents.size(), filenameUtf8.constData());
		m_textureCapping = new Graphic3d_Texture2D(imageCapping);
		m_textureCapping->EnableModulate();
		m_textureCapping->EnableRepeat();
		m_textureCapping->GetParams()->SetScale(Graphic3d_Vec2(0.05f, -0.05f));
	}
}



void Mx3dSectionTools::setRange(const Bnd_Box& bndBox)
{
	std::pair<double, double> xRange = Mx3d::getRange(Mx3d::BndBoxCoords::get(bndBox), gp::DX());
	std::pair<double, double> yRange = Mx3d::getRange(Mx3d::BndBoxCoords::get(bndBox), gp::DY());
	std::pair<double, double> zRange = Mx3d::getRange(Mx3d::BndBoxCoords::get(bndBox), gp::DZ());
	std::pair<double, double> customRange = Mx3d::getRange(Mx3d::BndBoxCoords::get(bndBox), gp_Dir(1, 1, 1));
	setXRange(xRange.first, xRange.second);
	setYRange(yRange.first, yRange.second);
	setZRange(zRange.first, zRange.second);
	setCustomRange(customRange.first, customRange.second);
}

void Mx3dSectionTools::setXRange(double a, double b)
{
	doubleSpinBox_xAxis->setRange(a, b);
	horizontalSlider_xAxis->setRange(a, b);
	horizontalSlider_xAxis->setValue((a + b) / 2.0);
}

void Mx3dSectionTools::setYRange(double a, double b)
{
	doubleSpinBox_yAxis->setRange(a, b);
	horizontalSlider_yAxis->setRange(a, b);
	horizontalSlider_yAxis->setValue((a + b) / 2.0);
}

void Mx3dSectionTools::setZRange(double a, double b)
{
	doubleSpinBox_zAxis->setRange(a, b);
	horizontalSlider_zAxis->setRange(a, b);
	horizontalSlider_zAxis->setValue((a + b) / 2.0);
}

void Mx3dSectionTools::setCustomRange(double a, double b)
{
	doubleSpinBox_custom->setRange(a, b);
	horizontalSlider_custom->setRange(a, b);
	horizontalSlider_custom->setValue((a + b) / 2.0);
	doubleSpinBox_custom_xDir->setValue(1.0);
	doubleSpinBox_custom_yDir->setValue(1.0);
	doubleSpinBox_custom_zDir->setValue(1.0);
}

void Mx3dSectionTools::setXClipPlanePosition(int pos)
{
	const gp_Dir& n = m_clipPlaneX->ToPlane().Axis().Direction();
	if (Mx3d::isReversedStandardDir(n))
		pos = -pos;

	const gp_Vec placement(pos * gp_Vec(n));
	m_clipPlaneX->SetEquation(gp_Pln(placement.XYZ(), n));
	emit sectionToolsChanged();
}

void Mx3dSectionTools::setYClipPlanePosition(int pos)
{
	const gp_Dir& n = m_clipPlaneY->ToPlane().Axis().Direction();
	if (Mx3d::isReversedStandardDir(n))
		pos = -pos;

	const gp_Vec placement(pos * gp_Vec(n));
	m_clipPlaneY->SetEquation(gp_Pln(placement.XYZ(), n));
	emit sectionToolsChanged();
}

void Mx3dSectionTools::setZClipPlanePosition(int pos)
{
	const gp_Dir& n = m_clipPlaneZ->ToPlane().Axis().Direction();
	if (Mx3d::isReversedStandardDir(n))
		pos = -pos;

	const gp_Vec placement(pos * gp_Vec(n));
	m_clipPlaneZ->SetEquation(gp_Pln(placement.XYZ(), n));
	emit sectionToolsChanged();
}

void Mx3dSectionTools::setCustomClipPlanePosition(int pos)
{
	const gp_Dir& n = m_clipPlaneCustom->ToPlane().Axis().Direction();
	if (Mx3d::isReversedStandardDir(n))
		pos = -pos;

	const gp_Vec placement(pos * gp_Vec(n));
	m_clipPlaneCustom->SetEquation(gp_Pln(placement.XYZ(), n));
	emit sectionToolsChanged();
}

void Mx3dSectionTools::setCustomClipPlaneDirection(const gp_Dir& dir)
{
	gp_Pln pln = m_clipPlaneCustom->ToPlane();
	gp_Pnt pt = pln.Axis().Location();
	pln.SetAxis(gp_Ax1(pt, dir));
	m_clipPlaneCustom->SetEquation(pln);
	emit sectionToolsChanged();
}

void Mx3dSectionTools::setXClipPlaneNormal(const gp_Dir& n)
{
	const double planePos = planePosition(m_clipPlaneX->ToPlane());
	const gp_Vec placement(planePos * gp_Vec(n));
	m_clipPlaneX->SetEquation(gp_Pln(placement.XYZ(), n));
}

void Mx3dSectionTools::setYClipPlaneNormal(const gp_Dir& n)
{
	const double planePos = planePosition(m_clipPlaneY->ToPlane());
	const gp_Vec placement(planePos * gp_Vec(n));
	m_clipPlaneY->SetEquation(gp_Pln(placement.XYZ(), n));
}

void Mx3dSectionTools::setZClipPlaneNormal(const gp_Dir& n)
{
	const double planePos = planePosition(m_clipPlaneZ->ToPlane());
	const gp_Vec placement(planePos * gp_Vec(n));
	m_clipPlaneZ->SetEquation(gp_Pln(placement.XYZ(), n));
}

void Mx3dSectionTools::setCustomClipPlaneNormal(const gp_Dir& n)
{
	const double planePos = planePosition(m_clipPlaneCustom->ToPlane());
	const gp_Vec placement(planePos * gp_Vec(n));
	m_clipPlaneCustom->SetEquation(gp_Pln(placement.XYZ(), n));
}



double Mx3dSectionTools::planePosition(const gp_Pln& plane)
{
	const gp_Vec vecLoc(plane.Location().XYZ());
	const gp_Vec vecNormal(plane.Axis().Direction());
	return vecLoc.Dot(vecNormal);
}