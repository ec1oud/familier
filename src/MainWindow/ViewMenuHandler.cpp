#include "ViewMenuHandler.h"
#include "SettingsManager.h"

ViewMenuHandler::ViewMenuHandler(Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui),
	_treeModel (dynamic_cast<TreeModel*>(ui->treeView->model ()))
{
	connect(ui->action_Expand, SIGNAL(triggered()), this, SLOT(expandCurrentEntry()));
	connect(ui->action_Collapse, SIGNAL(triggered()), this, SLOT(collapseCurrentEntry()));
	connect(ui->action_Expand_All_Siblings, SIGNAL(triggered()),
		this, SLOT(expandAllSiblingEntries()));
	connect(ui->action_Collapse_All_Siblings, SIGNAL(triggered()),
		this, SLOT(collapseAllSiblingEntries()));
	connect(ui->action_Expand_All, SIGNAL(triggered()), this, SLOT(expandAllEntries()));
	connect(ui->action_Collapse_All, SIGNAL(triggered()), this, SLOT(collapseAllEntries()));

	connect (ui->action_LockOrUnlock, SIGNAL(triggered()), this, SLOT(lockOrUnlockMainWindow()));

	switch (SettingsManager::s_LastCurrentView)
	{
	case SettingsManager::e_TreeView:
		switchViewToTreeView ();
		ui->tabWidget->setCurrentIndex (ui->tabWidget->indexOf (ui->tabFamilyNodeView));
		break;
	case SettingsManager::e_TableView:
		switchViewToTableView ();
		ui->tabWidget->setCurrentIndex (ui->tabWidget->indexOf (ui->tabFamilyNodeView));
		break;
	case SettingsManager::e_ListView:
		switchViewToListView ();
		ui->tabWidget->setCurrentIndex (ui->tabWidget->indexOf (ui->tabFamilyNodeView));
		break;
	case SettingsManager::e_IconView:
		switchViewToIconView ();
		ui->tabWidget->setCurrentIndex (ui->tabWidget->indexOf (ui->tabFamilyNodeView));
		break;
	case SettingsManager::e_FamilyTreeView:
		switchViewToFamilyTreeViewTab ();
		ui->tabWidget->setCurrentIndex (ui->tabWidget->indexOf (ui->tabFamilyTreeView));
		ui->stackedWidget->setCurrentIndex (ui->stackedWidget->indexOf (ui->pageTreeView));
		break;
	default:
		break;
	}

	ui->familyTreeView->setShowBasicDetailsForItem (SettingsManager::s_FamilyTreeShowBasicItemDetails);
	ui->familyTreeView->setShowExtraDetailsForItem (SettingsManager::s_FamilyTreeShowExtraItemDetails);

	connect (ui->action_Tree_View, SIGNAL(triggered()), this, SLOT(switchViewToTreeView()));
	connect (ui->action_Table_View, SIGNAL(triggered()), this, SLOT(switchViewToTableView()));
	connect (ui->action_List_View, SIGNAL(triggered()), this, SLOT(switchViewToListView()));
	connect (ui->action_Icon_View, SIGNAL(triggered()), this, SLOT(switchViewToIconView()));
	connect (ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleTabSwitch(int)));
}

void ViewMenuHandler::setLocked (bool locked)
{
	if (locked)
		ui->action_LockOrUnlock->setText ("Unlock Main Window");
	else
		ui->action_LockOrUnlock->setText ("Lock Main Window");
}

void ViewMenuHandler::setCurrentEntryExpanded(bool expand)
{
	switch (SettingsManager::s_LastCurrentView)
	{
	case SettingsManager::e_FamilyTreeView:
		{
			QModelIndex currentIndex = ui->familyTreeView->currentIndex ();
			if (!currentIndex.isValid())
				return;

			if (expand)
				ui->familyTreeView->expand (currentIndex);
			else
				ui->familyTreeView->collapse (currentIndex);
		}
		break;
	case SettingsManager::e_TreeView:
		{
			QModelIndex currentIndex = ui->treeView->currentIndex ();
			if (!currentIndex.isValid())
				return;

			if (expand)
				ui->treeView->expand (currentIndex);
			else
				ui->treeView->collapse (currentIndex);
		}
		break;
	default:
		return;
	}
}

void ViewMenuHandler::setAllSiblingEntriesExpanded(bool expand)
{
	switch (SettingsManager::s_LastCurrentView)
	{
	case SettingsManager::e_FamilyTreeView:
		{
			QModelIndex currentIndex = ui->familyTreeView->currentIndex ();
			if (!currentIndex.isValid())
				return;

			QAbstractItemModel *model = const_cast<QAbstractItemModel*>(currentIndex.model ());
			QModelIndex parentIndex = currentIndex.parent ();

			for (int row = 0; row < model->rowCount (parentIndex); row ++)
			{
				QModelIndex childIdx = model->index (row, 0, parentIndex);
				if (expand)
					ui->familyTreeView->expand (childIdx);
				else
					ui->familyTreeView->collapse (childIdx);
			}
		}
		break;
	case SettingsManager::e_TreeView:
		{
			QModelIndex currentIndex = ui->treeView->currentIndex ();
			if (!currentIndex.isValid())
				return;

			QAbstractItemModel *model = const_cast<QAbstractItemModel*>(currentIndex.model ());
			QModelIndex parentIndex = currentIndex.parent ();

			for (int row = 0; row < model->rowCount (parentIndex); row ++)
			{
				QModelIndex childIdx = model->index (row, 0, parentIndex);
				if (expand)
					ui->treeView->expand (childIdx);
				else
					ui->treeView->collapse (childIdx);
			}
		}
		break;
	default:
		return;
	}
}

void ViewMenuHandler::resizeToContents ()
{
	for (int column = 0; column < _treeModel->columnCount(); ++column)
		ui->treeView->resizeColumnToContents(column);

	ui->tableView->resizeColumnsToContents();
	ui->tableView->resizeRowsToContents();
}

void ViewMenuHandler::expandCurrentEntry()
{
	setCurrentEntryExpanded(true);
	ui->action_Expand->setDisabled(true);
	ui->action_Collapse->setDisabled(false);
}

void ViewMenuHandler::collapseCurrentEntry()
{
	setCurrentEntryExpanded(false);
	ui->action_Expand->setDisabled(false);
	ui->action_Collapse->setDisabled(true);
}

void ViewMenuHandler::expandAllSiblingEntries()
{
	setAllSiblingEntriesExpanded(true);
}

void ViewMenuHandler::collapseAllSiblingEntries()
{
	setAllSiblingEntriesExpanded(false);
}

void ViewMenuHandler::expandAllEntries()
{
	switch (SettingsManager::s_LastCurrentView)
	{
	case SettingsManager::e_FamilyTreeView:
		ui->familyTreeView->expandAll ();
		break;
	case SettingsManager::e_TreeView:
		ui->treeView->expandAll ();
		resizeToContents ();
		break;
	default:
		return;
	}
}

void ViewMenuHandler::collapseAllEntries()
{
	switch (SettingsManager::s_LastCurrentView)
	{
	case SettingsManager::e_FamilyTreeView:
		ui->familyTreeView->collapseAll ();;
		break;
	case SettingsManager::e_TreeView:
		ui->treeView->collapseAll ();;
		resizeToContents ();
		break;
	default:
		return;
	}
}

void ViewMenuHandler::lockOrUnlockMainWindow ()
{
	if (SettingsManager::s_MainWindowIsLocked)
		emit unlockMainWindow ();
	else
		emit lockAndMinimizeMainWindow ();
}

void ViewMenuHandler::switchViewToTreeView()
{
	int index = ui->stackedWidget->indexOf(ui->pageTreeView);
	ui->stackedWidget->setCurrentIndex(index);

	ui->action_Tree_View->setDisabled(true);
	ui->action_Table_View->setDisabled(false);
	ui->action_List_View->setDisabled(false);
	ui->action_Icon_View->setDisabled(false);

	ui->treeView->setFocus ();
	SettingsManager::s_LastCurrentView = SettingsManager::e_TreeView;
}

void ViewMenuHandler::switchViewToTableView()
{
	int index = ui->stackedWidget->indexOf(ui->pageTableView);
	ui->stackedWidget->setCurrentIndex(index);

	ui->action_Tree_View->setDisabled(false);
	ui->action_Table_View->setDisabled(true);
	ui->action_List_View->setDisabled(false);
	ui->action_Icon_View->setDisabled(false);

	ui->tableView->setFocus ();
	SettingsManager::s_LastCurrentView = SettingsManager::e_TableView;
}

void ViewMenuHandler::switchViewToListView()
{
	int index = ui->stackedWidget->indexOf(ui->pageListView);
	ui->stackedWidget->setCurrentIndex(index);

	ui->action_Tree_View->setDisabled(false);
	ui->action_Table_View->setDisabled(false);
	ui->action_List_View->setDisabled(true);
	ui->action_Icon_View->setDisabled(false);

	ui->listView->setViewMode(QListView::ListMode);
	ui->listView->setWrapping (false);
	ui->listView->setIconSize(QSize(64, 64));
	ui->listView->setSpacing(10);
	ui->listView->setGridSize (QSize (80,80));
	ui->listView->setFocus ();
	SettingsManager::s_LastCurrentView = SettingsManager::e_ListView;
}

void ViewMenuHandler::switchViewToIconView()
{
	int index = ui->stackedWidget->indexOf(ui->pageListView);
	ui->stackedWidget->setCurrentIndex(index);

	ui->action_Tree_View->setDisabled(false);
	ui->action_Table_View->setDisabled(false);
	ui->action_List_View->setDisabled(false);
	ui->action_Icon_View->setDisabled(true);

	ui->listView->setViewMode(QListView::IconMode);
	ui->listView->setMovement(QListView::Static);
	ui->listView->setWrapping (true);
	ui->listView->setIconSize(QSize(64, 64));
	ui->listView->setGridSize(QSize(128, 128));
	ui->listView->setSpacing(10);
	ui->listView->setFocus ();
	SettingsManager::s_LastCurrentView = SettingsManager::e_IconView;
}

void ViewMenuHandler::switchViewToFamilyTreeViewTab ()
{
	ui->action_Tree_View->setDisabled(true);
	ui->action_Table_View->setDisabled(true);
	ui->action_List_View->setDisabled(true);
	ui->action_Icon_View->setDisabled(true);

	ui->familyTreeView->setFocus ();
	SettingsManager::s_LastCurrentView = SettingsManager::e_FamilyTreeView;
}

void ViewMenuHandler::handleTabSwitch (int currentIndex)
{
	if (currentIndex == ui->tabWidget->indexOf (ui->tabFamilyTreeView))
		switchViewToFamilyTreeViewTab ();
	else if (currentIndex == ui->tabWidget->indexOf (ui->tabFamilyNodeView))
	{
		int stackedWidgetIndex = ui->stackedWidget->currentIndex ();

		if (stackedWidgetIndex == ui->stackedWidget->indexOf(ui->pageTreeView))
			switchViewToTreeView ();
		else if (stackedWidgetIndex == ui->stackedWidget->indexOf(ui->pageTableView))
			switchViewToTableView ();
		else if (stackedWidgetIndex == ui->stackedWidget->indexOf(ui->pageListView))
		{
			if (ui->listView->viewMode () == QListView::IconMode)
				switchViewToIconView ();
			else
				switchViewToListView ();
		}
	}
}

void ViewMenuHandler::regenerateTree()
{
	emit modelRequiresRegeneration();
}
