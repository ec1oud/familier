#include "EditFamilyDialog.h"
#include <QPushButton>

EditFamilyDialog::EditFamilyDialog (TreeModel *model, TreeItem *item, QWidget *parent)
	: EditDialog(dynamic_cast<Family*>(item->fam ())->getAliases(), parent),
	ui (new Ui::EditFamilyDialog), _model (model), _item (item),
	_family (dynamic_cast<Family*>(_item->fam ()))
{
	ui->setupUi(this);
	setupUi();
}

EditFamilyDialog::~EditFamilyDialog()
{
	delete ui;
}

void EditFamilyDialog::setupUi()
{
	setWindowTitle(QString("Editing Family: %1 (%2)").arg(_family->getName()).arg(_family->getId()));

	QModelIndex index = _model->index (_item, Familier::e_col_name);
	ui->lineEditName->setText(_model->data (index, Qt::EditRole).toString ());

	index = _model->index (_item, Familier::e_col_dummyOrBase);
	if (_model->data (index, Qt::EditRole).toBool ())
		ui->radioButtonBaseYes->setChecked(true);
	else
		ui->radioButtonBaseNo->setChecked(true);


	_aliasesListView = ui->listViewAliases;
	ui->listViewAliases->setModel(_aliasesModel);
	connect (ui->toolButtonAdd, SIGNAL(clicked()), this, SLOT(addAlias()));
	connect (ui->toolButtonDel, SIGNAL(clicked()), this, SLOT(delAlias()));
	connect (ui->toolButtonUp, SIGNAL(clicked()), this, SLOT(movAliasUp()));
	connect (ui->toolButtonDown, SIGNAL(clicked()), this, SLOT(movAliasDn()));

	ui->labelFamilyMembers->setText(_family->getListOfAnchors(Family::primary_members, false, false));

	ui->labelCreatedOnValue->setText(_family->getCreationTime().toString());
	ui->labelModifiedOnValue->setText(_family->getModificationTime().toString());
	ui->lcdNumberModificationCount->display(static_cast<int>(_family->getModificationCount()));

	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

	connect (ui->lineEditName, SIGNAL(textChanged(QString)), this, SLOT(setEdited()));
	connect (ui->radioButtonBaseYes, SIGNAL(toggled(bool)), this, SLOT(setEdited()));
	connect (ui->radioButtonBaseNo, SIGNAL(toggled(bool)), this, SLOT(setEdited()));
}

bool EditFamilyDialog::save()
{
	if (isEdited())
	{
		QModelIndex index = _model->index (_item, Familier::e_col_name);
		_model->setData (index, ui->lineEditName->text (), Qt::EditRole);
		index = _model->index (_item, Familier::e_col_dummyOrBase);
		_model->setData (index, ui->radioButtonBaseYes->isChecked (), Qt::EditRole);
		_family->setAliases(_aliasesModel->getData());
	}

	return isEdited();
}

void EditFamilyDialog::setEdited()
{
	EditDialog::setEdited();
	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
}
