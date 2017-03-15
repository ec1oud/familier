#include "MainPreferences.h"
#include "ui_MainPreferences.h"

MainPreferences::MainPreferences(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MainPreferences)
{
	ui->setupUi(this);
}

MainPreferences::~MainPreferences()
{
	delete ui;
}

void MainPreferences::changeEvent(QEvent *e)
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
