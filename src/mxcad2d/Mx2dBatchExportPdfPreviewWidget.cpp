/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dBatchExportPdfPreviewWidget.h"
#include <QtWidgets>

Mx2dBatchExportPdfPreviewWidget::Mx2dBatchExportPdfPreviewWidget(const QPixmap& previewImage, const QString& title, QWidget* parent)
	: QWidget(parent), m_previewImage(previewImage), m_title(title)
{
	setupUi();
	// Hide buttons by default
	m_pEditButton->hide();
	m_pDeleteButton->hide();
}

Mx2dBatchExportPdfPreviewWidget::~Mx2dBatchExportPdfPreviewWidget() {}

void Mx2dBatchExportPdfPreviewWidget::setupUi()
{
	// Main vertical layout
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(5, 5, 5, 5);
	pMainLayout->setSpacing(10);

	// --- Preview image container ---
	// This Frame is key, it will serve as the parent control for buttons and positioning coordinate system
	QFrame* pPreviewFrame = new QFrame();
	pPreviewFrame->setFrameShape(QFrame::Box);
	pPreviewFrame->setFrameShadow(QFrame::Sunken);
	//    pPreviewFrame->setFixedSize(200, 150);
		// Set a simple layout for pPreviewFrame, only used to manage the preview image
	QVBoxLayout* pPreviewFrameLayout = new QVBoxLayout(pPreviewFrame);
	pPreviewFrameLayout->setContentsMargins(0, 0, 0, 0);

	m_pPreviewLabel = new QLabel();
	m_pPreviewLabel->setPixmap(m_previewImage.scaled(200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	m_pPreviewLabel->setAlignment(Qt::AlignCenter);
	pPreviewFrameLayout->addWidget(m_pPreviewLabel); // Preview image is managed by layout, will auto-scale

	// --- Edit and delete buttons ---
	// 1. Create buttons, key: specify pPreviewFrame as parent control!
	m_pEditButton = new QPushButton(pPreviewFrame);
	m_pDeleteButton = new QPushButton(pPreviewFrame);

	// Set icons and styles (same as before)
	m_pEditButton->setIcon(QIcon(":/resources/images2d/2d_editPdfPageName.svg"));
	m_pDeleteButton->setIcon(QIcon(":/resources/images2d/2d_deletePdfPreview.svg"));
	m_pEditButton->setFixedSize(24, 24);
	m_pDeleteButton->setFixedSize(24, 24);
	m_pEditButton->setCursor(Qt::PointingHandCursor);
	m_pDeleteButton->setCursor(Qt::PointingHandCursor);
	m_pEditButton->setFlat(true);
	m_pDeleteButton->setFlat(true);

	// 2. Manually set button positions
	// Place buttons in the top-right corner of pPreviewFrame
	// Note: Coordinates here are relative to parent control pPreviewFrame


	// --- Title section ---
	m_pTitleLabel = new QLabel(m_title);
	m_pTitleLabel->setAlignment(Qt::AlignCenter);

	// Add preview image container and title label to main layout
	pMainLayout->addWidget(pPreviewFrame);
	pMainLayout->addWidget(m_pTitleLabel);

	// Set fixed size
//    this->setFixedSize(220, 250);
	pPreviewFrame->adjustSize();
	int margin = 2;
	m_pDeleteButton->move(pPreviewFrame->width() - m_pDeleteButton->width() - margin, margin);
	m_pEditButton->move(pPreviewFrame->width() - m_pDeleteButton->width() - m_pEditButton->width() - margin, margin);

	m_pEditButton->hide();
	m_pDeleteButton->hide();
}
void Mx2dBatchExportPdfPreviewWidget::enterEvent(QEvent* event)
{
	// Show buttons when mouse enters
	m_pEditButton->show();
	m_pDeleteButton->show();
	QWidget::enterEvent(event);
}

void Mx2dBatchExportPdfPreviewWidget::leaveEvent(QEvent* event)
{
	// Hide buttons when mouse leaves
	m_pEditButton->hide();
	m_pDeleteButton->hide();
	QWidget::leaveEvent(event);
}