#include "ToolBarAndMenuHandler.h"
#include <QModelIndex>
#include "TreeItem.h"
#include "SettingsManager.h"
#include "FamilyTreeModel.h"
#include "ActionWithIdentity.h"

ToolBarAndMenuHandler::ToolBarAndMenuHandler(Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui),
	  _treeModel (dynamic_cast<TreeModel*>(ui->treeView->model ())),
	  _iconModel (dynamic_cast<IconModel*>(ui->listView->model ()))
{
}

void ToolBarAndMenuHandler::disableAllToolItemsForEditing ()
{
	ui->action_Edit_Inline->setDisabled(true);
	ui->action_Mark_as_Dead->setDisabled(true);
	ui->action_Mark_as_Alive->setDisabled(true);
	ui->action_Mark_as_Dummy->setDisabled(true);
	ui->action_Unmark_as_Dummy->setDisabled(true);

	ui->action_Edit->setDisabled(true);
	ui->action_Delete->setDisabled(true);

	ui->action_Add_Spouse->setDisabled(true);
	ui->action_Add_Dummy_Spouse->setDisabled (true);
	ui->action_Add_Male_Sibling->setDisabled(true);
	ui->action_Add_Female_Sibling->setDisabled(true);
	ui->action_Add_Wife_in_Law->setDisabled(true);
	ui->action_Add_Male_Child->setDisabled(true);
	ui->action_Add_Female_Child->setDisabled(true);

	ui->action_Expand->setDisabled(true);
	ui->action_Collapse->setDisabled(true);
}

void ToolBarAndMenuHandler::updateToolBarAndMenuItemsForIndexChange (QModelIndex currentIndex)
{
	/* Remember this may be called for any of the views.
	   NOTE: Don't use QModelIndex directly!!!
	 */

	bool expanded = false;
	bool isTreeView = false;
	switch (SettingsManager::s_LastCurrentView)
	{
	case SettingsManager::e_TreeView:
		isTreeView = true;
		expanded = ui->treeView->isExpanded (currentIndex);
		break;
	case SettingsManager::e_FamilyTreeView:
		isTreeView = true;
		expanded = ui->familyTreeView->isExpanded (currentIndex);
		break;
	default:
		isTreeView = false;
	}

	if (isTreeView)
	{
		ui->action_Expand_All_Siblings->setEnabled (true);
		ui->action_Collapse_All_Siblings->setEnabled (true);
		ui->action_Expand_All->setEnabled (true);
		ui->action_Collapse_All->setEnabled (true);
	}
	else
	{
		ui->action_Expand_All_Siblings->setDisabled (true);
		ui->action_Collapse_All_Siblings->setDisabled (true);
		ui->action_Expand_All->setDisabled (true);
		ui->action_Collapse_All->setDisabled (true);
	}

	if (!currentIndex.isValid())
		disableAllToolItemsForEditing ();
	else
	{
		if (Qt::ItemIsEditable & (currentIndex.model()->flags(currentIndex)))
			ui->action_Edit_Inline->setEnabled(true);
		else
			ui->action_Edit_Inline->setDisabled(true);

		TreeItem *item = NULL;
		QAbstractItemModel *currentModel = const_cast<QAbstractItemModel*>(currentIndex.model ());
		if (isTreeView)
			item = dynamic_cast<TreeModel*>(currentModel)->getItem (currentIndex);
		else
			item = dynamic_cast<TableModel*>(currentModel)->getItem (currentIndex);
		if (!item)
			return;

		ui->action_Edit->setEnabled(true);
		if (item->childCount())
			ui->action_Delete->setDisabled(true);
		else
			ui->action_Delete->setEnabled(true);

		if (isTreeView)
		{
			if (item->childCount())
			{
				if (expanded)
				{
					ui->action_Expand->setDisabled(true);
					ui->action_Collapse->setEnabled(true);
				}
				else
				{
					ui->action_Expand->setEnabled(true);
					ui->action_Collapse->setDisabled(true);
				}
			}
			else
			{
				ui->action_Expand->setDisabled(true);
				ui->action_Collapse->setDisabled(true);
			}
		}

		if (item->isIndividual())
		{
			Individual *individual = dynamic_cast<Individual*>(item->fam());

			if (individual->isDead())
			{
				ui->action_Mark_as_Dead->setDisabled(true);
				ui->action_Mark_as_Alive->setEnabled(true);
			}
			else
			{
				ui->action_Mark_as_Dead->setEnabled(true);
				ui->action_Mark_as_Alive->setDisabled(true);
			}

			ui->action_Add_Wife_in_Law->setDisabled(true);

			if (individual->isFemale())
			{
				if (individual->isDummy())
				{
					ui->action_Mark_as_Dummy->setDisabled(true);
					ui->action_Unmark_as_Dummy->setEnabled(true);
				}
				else
				{
					ui->action_Mark_as_Dummy->setEnabled(true);
					ui->action_Unmark_as_Dummy->setDisabled(true);
				}

				ui->action_Add_Spouse->setDisabled(true);
				ui->action_Add_Dummy_Spouse->setDisabled (true);

				if (item->isSpouseItem())
				{
					ui->action_Add_Male_Sibling->setDisabled(true);
					ui->action_Add_Female_Sibling->setDisabled(true);
					ui->action_Add_Wife_in_Law->setEnabled(true);

					ui->action_Add_Male_Child->setEnabled(true);
					ui->action_Add_Female_Child->setEnabled(true);
				}
				else
				{
					ui->action_Add_Male_Sibling->setEnabled(true);
					ui->action_Add_Female_Sibling->setEnabled(true);

					ui->action_Add_Male_Child->setDisabled(true);
					ui->action_Add_Female_Child->setDisabled(true);
				}
			}
			else
			{
				ui->action_Mark_as_Dummy->setDisabled(true);
				ui->action_Unmark_as_Dummy->setDisabled(true);

				ui->action_Add_Spouse->setEnabled(true);
				ui->action_Add_Dummy_Spouse->setEnabled (true);
				ui->action_Add_Male_Sibling->setEnabled(true);
				ui->action_Add_Female_Sibling->setEnabled(true);
				ui->action_Add_Male_Child->setDisabled(true);
				ui->action_Add_Female_Child->setDisabled(true);
			}
		}
		else
		{
			ui->action_Mark_as_Dead->setDisabled(true);
			ui->action_Mark_as_Alive->setDisabled(true);
			ui->action_Mark_as_Dummy->setDisabled(true);
			ui->action_Unmark_as_Dummy->setDisabled(true);

			ui->action_Add_Spouse->setDisabled(true);
			ui->action_Add_Dummy_Spouse->setDisabled (true);
			ui->action_Add_Male_Sibling->setDisabled(true);
			ui->action_Add_Female_Sibling->setDisabled(true);
			ui->action_Add_Wife_in_Law->setDisabled(true);
			ui->action_Add_Male_Child->setEnabled(true);
			ui->action_Add_Female_Child->setEnabled(true);
		}
	}
}

void ToolBarAndMenuHandler::customContextMenuRequestHandler (QPoint point)
{
	QMenu menu(dynamic_cast<QWidget*>(parent()));

	QModelIndex pointIndex;
	QAbstractItemView *currentView = SettingsManager::currentView ();
	if (!currentView)
		return;
	pointIndex = currentView->indexAt (point);
	updateToolBarAndMenuItemsForIndexChange(pointIndex);

	menu.addAction (ui->action_Edit_Inline);
	menu.addAction (ui->action_Mark_as_Dead);
	menu.addAction (ui->action_Mark_as_Alive);
	menu.addAction (ui->action_Mark_as_Dummy);
	menu.addAction (ui->action_Unmark_as_Dummy);
	menu.addSeparator ();
	menu.addAction (ui->action_Edit);
	menu.addAction (ui->action_Delete);

	if (SettingsManager::currentViewIsTreeView ())
	{
		menu.addSeparator();
		menu.addAction (ui->action_Add_Spouse);
		menu.addAction (ui->action_Add_Dummy_Spouse);
		menu.addAction (ui->action_Add_Male_Sibling);
		menu.addAction (ui->action_Add_Female_Sibling);
		menu.addAction (ui->action_Add_Wife_in_Law);
		menu.addAction (ui->action_Add_Male_Child);
		menu.addAction (ui->action_Add_Female_Child);
		menu.addSeparator ();
		menu.addAction (ui->action_Expand);
		menu.addAction (ui->action_Collapse);
		menu.addAction (ui->action_Expand_All_Siblings);
		menu.addAction (ui->action_Collapse_All_Siblings);
		menu.addSeparator ();
		menu.addAction (ui->action_Add_Family);

		TreeItem *item = SettingsManager::currentItemForCurrentView ();
		if (item && item->isIndividual ())
		{
			Individual *individual = dynamic_cast<Individual*>(item->fam ());
			if (individual->isFemale ())
			{
				menu.addSeparator ();
				if (item->isSpouseItem ())
				{
					if (individual->getFather ())
					{
						Individual *father = individual->getFather ();
						ActionWithIdentity *awi = new ActionWithIdentity (father->getId (), &menu);
						awi->setText ("Go to father: " + father->getName ());
						awi->setIcon (*father->getIcon ());
						menu.addAction (awi);
						connect (awi, SIGNAL(triggered(ulong)),
							 this, SLOT(handleAlterEgoClick(ulong)));
					}
				}
				for (int i = 0; i < individual->spouseCount (); i++)
				{
					Individual *spouse = individual->getSpouse (i);
					if (spouse->getId ()== item->parentItem ()->id ())
						continue;

					ActionWithIdentity *awi = new ActionWithIdentity (spouse->getId (), &menu);
					awi->setText ("Go to spouse: " + spouse->getName ());
					awi->setIcon (*spouse->getIcon ());
					menu.addAction (awi);
					connect (awi, SIGNAL(triggered(ulong)),
						 this, SLOT(handleAlterEgoClick(ulong)));
				}
			}
		}

		if (SettingsManager::s_LastCurrentView == SettingsManager::e_FamilyTreeView)
		{
			menu.addSeparator ();
			QAction *hideDetailsAction = new QAction ("Show extra details for items", &menu);
			hideDetailsAction->setCheckable (true);
			hideDetailsAction->setChecked (ui->familyTreeView->getShowExtraDetailsForItem ());
			menu.addAction (hideDetailsAction);
			connect (hideDetailsAction, SIGNAL(triggered(bool)),
				 ui->familyTreeView, SLOT(setShowExtraDetailsForItem(bool)));

			QAction *showNodesOnlyAction = new QAction ("Show basic details for items", &menu);
			showNodesOnlyAction->setCheckable (true);
			showNodesOnlyAction->setChecked (ui->familyTreeView->getShowBasicDetailsForItem ());
			menu.addAction (showNodesOnlyAction);
			connect (showNodesOnlyAction, SIGNAL(triggered(bool)),
				 ui->familyTreeView, SLOT(setShowBasicDetailsForItem(bool)));
		}
	}
	else if (SettingsManager::currentViewIsListView ())
	{
		menu.addSeparator ();
		QAction *hideDetailsAction = new QAction ("Show items without images", &menu);
		hideDetailsAction->setCheckable (true);
		hideDetailsAction->setChecked (_iconModel->shouldShowItemsWOImagesInIconView ());
		menu.addAction (hideDetailsAction);
		connect (hideDetailsAction, SIGNAL(triggered(bool)),
			 _iconModel, SLOT(setShouldShowItemsWOImagesInIconView(bool)));
	}

	menu.exec(QCursor::pos());
}

void ToolBarAndMenuHandler::handleAlterEgoClick (ulong id)
{
	foreach (TreeItem *item, TreeItem::individualTreeItemList)
	{
		if (item->id () == id)
			emit needToScrollTo (_treeModel->index (item, Familier::e_col_maximum));
	}
}
