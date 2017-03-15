#include "NavigateMenuHandler.h"
#include "SettingsManager.h"

NavigateMenuHandler::NavigateMenuHandler(Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui),
	_treeModel (dynamic_cast<TreeModel*>(ui->treeView->model ())),
	_tableModel (dynamic_cast<TableModel*>(ui->tableView->model ())),
	_iconModel (dynamic_cast<IconModel*>(ui->listView->model ())),
	_familyTreeModel (dynamic_cast<FamilyTreeModel*>(ui->familyTreeView->model ()))
{
	currentActiveIndex = -1;

	connect(ui->action_Go_to_Previous, SIGNAL(triggered()), this, SLOT(gotoPrevious()));
	connect(ui->action_Go_to_Next, SIGNAL(triggered()), this, SLOT(gotoNext()));
}

void NavigateMenuHandler::updateGotoActionItems()
{
	if (currentActiveIndex > 0)
		ui->action_Go_to_Previous->setEnabled(true);
	else
		ui->action_Go_to_Previous->setEnabled(false);

	if (currentActiveIndex < lastActiveIndexsVector.size() -1)
		ui->action_Go_to_Next->setEnabled(true);
	else
		ui->action_Go_to_Next->setEnabled(false);
}

QStringList NavigateMenuHandler::getOpenedItemsAsUrls ()
{
	QStringList urlList;

	foreach (QModelIndex index, lastActiveIndexsVector)
	{
		TreeItem *item = _treeModel->getItem (index);
		if (item->isIndividual ())
			urlList << QString ("iid:%1#%2").arg (item->id ())
				   .arg (item->isSpouseItem () ? item->parentItem ()->id () : 0);
		else
			urlList << QString ("fid:%1").arg (item->id ());
	}

	return urlList;
}

void NavigateMenuHandler::scrollTo(QModelIndex index)
{
	if (!index.isValid())
		return;

	ui->treeView->scrollTo(index, QAbstractItemView::PositionAtCenter);
	ui->treeView->setCurrentIndex(index);

	QModelIndex tmIndex = _tableModel->mapFromSource(index);
	ui->tableView->scrollTo(tmIndex, QAbstractItemView::PositionAtCenter);
	ui->tableView->setCurrentIndex(tmIndex);

	QModelIndex imIndex = _iconModel->mapFromSource(index);
	ui->listView->scrollTo(imIndex, QAbstractItemView::PositionAtCenter);
	ui->listView->setCurrentIndex(imIndex);

	QModelIndex ftmIndex = _familyTreeModel->mapFromSource (index);
	ui->familyTreeView->scrollTo (ftmIndex, QAbstractItemView::PositionAtCenter);
	ui->familyTreeView->setCurrentIndex (ftmIndex);
}

void NavigateMenuHandler::gotoPrevious()
{
	if (currentActiveIndex <= 0)
		return;

	QModelIndex jumpToIndex = lastActiveIndexsVector [currentActiveIndex - 1];
	if (jumpToIndex.isValid())
	{
		currentActiveIndex--;
		scrollTo(jumpToIndex);
		updateGotoActionItems();
		emit itemViewNeedsUpdation(jumpToIndex);
	}
	else
	{
		lastActiveIndexsVector.remove(currentActiveIndex - 1);
		gotoPrevious();
	}
}

void NavigateMenuHandler::gotoNext()
{
	if (currentActiveIndex >= lastActiveIndexsVector.size() - 1)
		return;

	QModelIndex jumpToIndex = lastActiveIndexsVector [currentActiveIndex + 1];
	if (jumpToIndex.isValid())
	{
		currentActiveIndex++;
		scrollTo(jumpToIndex);
		updateGotoActionItems();
		emit itemViewNeedsUpdation(jumpToIndex);
	}
	else
	{
		lastActiveIndexsVector.remove(currentActiveIndex + 1);
		gotoNext();
	}
}

void NavigateMenuHandler::appendIntoLastActiveIndexes(QModelIndex index)
{
	int size = lastActiveIndexsVector.size();
	for (int i = size; i > currentActiveIndex + 1; i--)
		lastActiveIndexsVector.pop_back();

	lastActiveIndexsVector.append (index);
	currentActiveIndex = lastActiveIndexsVector.size() - 1;

	updateGotoActionItems();
}

void NavigateMenuHandler::clearLastActiveIndexes()
{
	lastActiveIndexsVector.clear();
	currentActiveIndex = -1;

	ui->action_Go_to_Next->setDisabled(true);
	ui->action_Go_to_Previous->setDisabled(true);
}
