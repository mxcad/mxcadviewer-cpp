/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dAreaOffsetDialog.h"
#include "Mx2dUtils.h"
#include "MxUtils.h"


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QDialogButtonBox>

Mx2dAreaOffsetDialog::Mx2dAreaOffsetDialog(QWidget* parent)
	: QDialog(parent)
{
	setupUi();
	connectSignals();
}

Mx2dAreaOffsetDialog::~Mx2dAreaOffsetDialog()
{
}

void Mx2dAreaOffsetDialog::setAreaMarkId(const McDbObjectId& id)
{
	m_areaMarkId = id;
}

void Mx2dAreaOffsetDialog::setupUi()
{
	setWindowTitle(tr("Area Offset"));
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(15);
	mainLayout->setContentsMargins(15, 15, 15, 15);


	auto* formLayout = new QFormLayout();
	formLayout->setLabelAlignment(Qt::AlignRight);

	m_offsetValueEdit = new QLineEdit();

	auto* validator = new QDoubleValidator(this);
	validator->setBottom(0.0); // Only allow non-negative numbers
	validator->setNotation(QDoubleValidator::StandardNotation);
	m_offsetValueEdit->setValidator(validator);
	m_offsetValueEdit->setPlaceholderText(tr("Enter a positive number"));

	formLayout->addRow(tr("Offset Value:"), m_offsetValueEdit);


	auto* offsetTypeGroup = new QGroupBox(tr("Offset Type"));
	auto* typeLayout = new QHBoxLayout(offsetTypeGroup);
	m_expandRadio = new QRadioButton(tr("Expand"));
	m_shrinkRadio = new QRadioButton(tr("Shrink"));
	m_expandRadio->setChecked(true); // Default to "Expand"

	typeLayout->addWidget(m_expandRadio);
	typeLayout->addWidget(m_shrinkRadio);
	typeLayout->addStretch();

	auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	m_offsetButton = buttonBox->button(QDialogButtonBox::Ok);
	m_offsetButton->setText(tr("Offset"));


	mainLayout->addLayout(formLayout);
	mainLayout->addWidget(offsetTypeGroup);
	mainLayout->addStretch();
	mainLayout->addWidget(buttonBox, 0, Qt::AlignRight);

	// Adjust size to fit contents and then fix it.
	adjustSize();
	setFixedSize(size());
}

void Mx2dAreaOffsetDialog::connectSignals()
{

	connect(m_offsetButton, &QPushButton::clicked, this, &Mx2dAreaOffsetDialog::onOffsetButtonClicked);

	// The Cancel button is automatically connected to reject() by QDialogButtonBox.
	auto* buttonBox = findChild<QDialogButtonBox*>();
	if (buttonBox) {
		connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	}
}

void Mx2dAreaOffsetDialog::onOffsetButtonClicked()
{
	bool isConversionOk;
	double value = m_offsetValueEdit->text().toDouble(&isConversionOk);

	if (!isConversionOk) {
		QMessageBox::warning(this, tr("Invalid Input"), tr("Please enter a valid number."));
		m_offsetValueEdit->selectAll();
		m_offsetValueEdit->setFocus();
		return;
	}

	if (value <= 0) {
		QMessageBox::warning(this, tr("Invalid Input"), tr("The offset value must be greater than zero."));
		m_offsetValueEdit->selectAll();
		m_offsetValueEdit->setFocus();
		return;
	}

	// --- Execute CAD Command ---
	MrxDbgRbList spParam = Mx::mcutBuildList(
		RTREAL, value,
		RTId, m_areaMarkId.asOldId(),
		RTSHORT, static_cast<short>(offsetType()), 0);

	Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DoOffsetArea", spParam.orphanData());

}

double Mx2dAreaOffsetDialog::offsetValue() const
{
	return m_offsetValueEdit->text().toDouble();
}

Mx2dAreaOffsetDialog::OffsetType Mx2dAreaOffsetDialog::offsetType() const
{
	return m_expandRadio->isChecked() ? OffsetType::Expand : OffsetType::Shrink;
}