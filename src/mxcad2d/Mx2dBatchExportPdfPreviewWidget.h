/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

class Mx2dBatchExportPdfPreviewWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Mx2dBatchExportPdfPreviewWidget(const QPixmap& previewImage, const QString& title, QWidget* parent = nullptr);
	~Mx2dBatchExportPdfPreviewWidget();

protected:
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;

private:
	void setupUi();

	QLabel* m_pPreviewLabel;
	QLabel* m_pTitleLabel;
	QPushButton* m_pEditButton;
	QPushButton* m_pDeleteButton;

	QPixmap m_previewImage;
	QString m_title;
};

