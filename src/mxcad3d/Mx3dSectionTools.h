/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QWidget>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <V3d_View.hxx>
#include <Bnd_Box.hxx>
#include <gp_Dir.hxx>
#include "Mx3dUtils.h"

class QCheckBox;
class QDoubleSpinBox;
class QPushButton;
class QSlider;
class QWidget;
class QGridLayout;
class QHBoxLayout;

class Mx3dView;

class Mx3dSectionTools : public QWidget
{
	Q_OBJECT

public:
	Mx3dSectionTools(QWidget* parent);
	~Mx3dSectionTools();

	std::vector<Handle_Graphic3d_ClipPlane> getClipPlanes();

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	void initUi();

	void connectSignals();
	void connectSignalsX();
	void connectSignalsY();
	void connectSignalsZ();
	void connectSignalsCustom();
	bool getCustomDirection(gp_Dir& dir);
	void setXEnabled(bool isEnabled);
	void setYEnabled(bool isEnabled);
	void setZEnabled(bool isEnabled);
	void setCustomEnabled(bool isEnabled);

private:
	void initClipPlanes();

	void createPlaneCappingTexture();
	void setXRange(double a, double b);
	void setYRange(double a, double b);
	void setZRange(double a, double b);
	void setCustomRange(double a, double b);

private:
	void setXClipPlanePosition(int pos);
	void setYClipPlanePosition(int pos);
	void setZClipPlanePosition(int pos);
	void setCustomClipPlanePosition(int pos);
	void setCustomClipPlaneDirection(const gp_Dir& dir);
	void setXClipPlaneNormal(const gp_Dir& n);
	void setYClipPlaneNormal(const gp_Dir& n);
	void setZClipPlaneNormal(const gp_Dir& n);
	void setCustomClipPlaneNormal(const gp_Dir& n);

private:
	double planePosition(const gp_Pln& plane);

public:
	void setRange(const Bnd_Box& bndBox);

private:
	QGridLayout* gridLayout_2;

	QCheckBox* checkBox_xAxis;
	QWidget* widget_X;
	QHBoxLayout* horizontalLayout;
	QDoubleSpinBox* doubleSpinBox_xAxis;
	QSlider* horizontalSlider_xAxis;
	QPushButton* btn_xAxis;

	QCheckBox* checkBox_yAxis;
	QWidget* widget_Y;
	QHBoxLayout* horizontalLayout_2;
	QDoubleSpinBox* doubleSpinBox_yAxis;
	QSlider* horizontalSlider_yAxis;
	QPushButton* btn_yAxis;

	QCheckBox* checkBox_zAxis;
	QWidget* widget_Z;
	QHBoxLayout* horizontalLayout_3;
	QDoubleSpinBox* doubleSpinBox_zAxis;
	QSlider* horizontalSlider_zAxis;
	QPushButton* btn_zAxis;

	QCheckBox* checkBox_custom;
	QWidget* widget_Custom;
	QGridLayout* gridLayout;
	QDoubleSpinBox* doubleSpinBox_custom;
	QSlider* horizontalSlider_custom;
	QPushButton* btn_custom;
	QHBoxLayout* horizontalLayout_4;
	QDoubleSpinBox* doubleSpinBox_custom_xDir;
	QDoubleSpinBox* doubleSpinBox_custom_yDir;
	QDoubleSpinBox* doubleSpinBox_custom_zDir;

	// OpenCascade Members
	Handle_Graphic3d_ClipPlane              m_clipPlaneX;
	Handle_Graphic3d_ClipPlane              m_clipPlaneY;
	Handle_Graphic3d_ClipPlane              m_clipPlaneZ;
	Handle_Graphic3d_ClipPlane              m_clipPlaneCustom;
	std::vector<Handle_Graphic3d_ClipPlane> m_vecClipPlaneData;
	Handle_Graphic3d_TextureMap             m_textureCapping;

signals:
	void sectionToolsChanged();
};