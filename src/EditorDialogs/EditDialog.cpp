#include "EditDialog.h"
#include <QMessageBox>

EditDialog::EditDialog(QStringList aliases, QWidget *parent) :
		QDialog(parent), _aliasesModel (new StringListModel (aliases))
{
	_edited = false;
}

EditDialog::~EditDialog()
{
	delete _aliasesModel;
}

void EditDialog::setEdited()
{
	_edited = true;
}

void EditDialog::addAlias()
{
	if (_aliasesModel->insertRow(_aliasesModel->rowCount(QModelIndex())))
	{
		QModelIndex index = _aliasesModel->index(_aliasesModel->rowCount(QModelIndex()) - 1,
							 0, QModelIndex());
		_aliasesListView->scrollTo(index);
		_aliasesListView->edit(index);
		setEdited();
	}
	else
	{
		QMessageBox::warning(this, "Error", "Adding of new event failed.");
		return;
	}
}

void EditDialog::delAlias()
{
	QModelIndex index = _aliasesListView->currentIndex();
	if (!index.isValid())
	{
		QMessageBox::warning(this, "Error", "No alias selected");
		return;
	}

	if (QMessageBox::question(this, "Delete alias",
				  "Are you sure you want to delete selected alias?",
				  QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		if (_aliasesModel->removeRow(index.row(), QModelIndex()))
			setEdited();
	}
}

void EditDialog::movAliasUp()
{
	QModelIndex index = _aliasesListView->currentIndex();
	if (!index.isValid())
	{
		QMessageBox::warning(this, "Error", "No alias selected");
		return;
	}

	QModelIndex dstIndex = _aliasesModel->index(index.row() - 1, 0, QModelIndex());
	QString srcString = _aliasesModel->data(index, Qt::DisplayRole).toString();
	QString dstString = _aliasesModel->data(dstIndex, Qt::DisplayRole).toString();
	_aliasesModel->setData(index, dstString, Qt::EditRole);
	_aliasesModel->setData(dstIndex, srcString, Qt::EditRole);
}

void EditDialog::movAliasDn()
{
	QModelIndex index = _aliasesListView->currentIndex();
	if (!index.isValid())
	{
		QMessageBox::warning(this, "Error", "No alias selected");
		return;
	}

	QModelIndex dstIndex = _aliasesModel->index(index.row() + 1, 0, QModelIndex());
	QString srcString = _aliasesModel->data(index, Qt::DisplayRole).toString();
	QString dstString = _aliasesModel->data(dstIndex, Qt::DisplayRole).toString();
	_aliasesModel->setData(index, dstString, Qt::EditRole);
	_aliasesModel->setData(dstIndex, srcString, Qt::EditRole);
}
