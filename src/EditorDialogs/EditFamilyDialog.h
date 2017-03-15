#ifndef CLASSEDITFAMILYDIALOG_H
#define CLASSEDITFAMILYDIALOG_H

#include "EditDialog.h"
#include "ui_EditFamily.h"
#include "Family.h"
#include "TreeModel.h"

class EditFamilyDialog : public EditDialog
{
	Q_OBJECT

	Ui::EditFamilyDialog *		ui;
	TreeModel *			_model;
	TreeItem *			_item;
	Family *			_family;

public:
	EditFamilyDialog (TreeModel *model, TreeItem *item, QWidget *parent = 0);
	~EditFamilyDialog ();

private:
	virtual void	setupUi				();

public:
	virtual bool	save				();

private slots:
	virtual void	setEdited			();
};

#endif // CLASSEDITFAMILYDIALOG_H
