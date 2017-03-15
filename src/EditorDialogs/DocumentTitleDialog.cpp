#include "DocumentTitleDialog.h"

DocumentTitleDialog::DocumentTitleDialog(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::DocumentTitleDialog)
{
	ui->setupUi(this);
}

DocumentTitleDialog::~DocumentTitleDialog()
{
	delete ui;
}

void DocumentTitleDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
