#ifndef DOCUMENTTITLEDIALOG_H
#define DOCUMENTTITLEDIALOG_H

#include <QDialog>
#include "ui_DocumentTitleDialog.h"

namespace Ui
{
	class DocumentTitleDialog;
}

class DocumentTitleDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DocumentTitleDialog(QWidget *parent = 0);
	~DocumentTitleDialog();

protected:
	void changeEvent(QEvent *e);

public:
	Ui::DocumentTitleDialog *ui;
};

#endif // DOCUMENTTITLEDIALOG_H
