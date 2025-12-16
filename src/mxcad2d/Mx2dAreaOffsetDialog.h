/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include "MxCADInclude.h"

// Forward declarations for UI elements
QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QGroupBox;
class QRadioButton;
class QPushButton;
QT_END_NAMESPACE

/**
 * @class Mx2dAreaOffsetDialog
 * @brief A dialog to get user input for offsetting a 2D area.
 *
 * This dialog prompts the user for an offset value and a direction (expand or shrink).
 * It validates the input and provides public methods to retrieve the results.
 */
	class Mx2dAreaOffsetDialog : public QDialog
{
	Q_OBJECT

public:
	// Defines the offset direction.
	enum class OffsetType
	{
		Expand = 1,
		Shrink = 2
	};

	explicit Mx2dAreaOffsetDialog(QWidget* parent = nullptr);
	~Mx2dAreaOffsetDialog() override;

	// Sets the database ID of the area mark to be offset.
	void setAreaMarkId(const McDbObjectId& id);

	// Retrieves the user-entered offset value.
	double offsetValue() const;

	// Retrieves the user-selected offset type.
	OffsetType offsetType() const;

private:
	void setupUi();
	void connectSignals();

private slots:
	void onOffsetButtonClicked();

private:
	QLineEdit* m_offsetValueEdit;
	QRadioButton* m_expandRadio;
	QRadioButton* m_shrinkRadio;
	QPushButton* m_offsetButton;


	McDbObjectId    m_areaMarkId;
};