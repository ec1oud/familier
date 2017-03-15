#include "EditMenuHandler.h"
#include "EditFamilyDialog.h"
#include "EditIndividualDialog.h"
#include <QMessageBox>
#include "EventModel.h"
#include "DocumentTitleDialog.h"
#include "SettingsManager.h"
#include "MainPreferences.h"
#include <QInputDialog>
#include <QScrollBar>

EditMenuHandler::EditMenuHandler(FamilyModel *familyModel, EventModel *eventModel,
				 Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui),
	  _treeModel (dynamic_cast<TreeModel*>(ui->treeView->model ())),
	  _tableModel (dynamic_cast<TableModel*>(ui->tableView->model ())),
	  _iconModel (dynamic_cast<IconModel*>(ui->listView->model ())),
	  _familyTreeModel (dynamic_cast<FamilyTreeModel*>(ui->familyTreeView->model ())),
	  _familyModel (familyModel), _eventModel (eventModel)
{
	connect (ui->action_Edit_Inline, SIGNAL(triggered()), this, SLOT(editInline()));
	connect (ui->action_Mark_as_Dead, SIGNAL(triggered()), this, SLOT(markAsDead()));
	connect (ui->action_Mark_as_Alive, SIGNAL(triggered()), this, SLOT(markAsAlive()));
	connect (ui->action_Mark_as_Dummy, SIGNAL(triggered()), this, SLOT(markAsDummy()));
	connect (ui->action_Unmark_as_Dummy, SIGNAL(triggered()), this, SLOT(unmarkAsDummy()));
	connect (ui->action_Edit, SIGNAL(triggered()), this, SLOT(editEntry()));
	connect (ui->action_Delete, SIGNAL(triggered()), this, SLOT(delEntry()));

	connect (ui->action_Add_Spouse, SIGNAL(triggered()), this, SLOT(addNewSpouse()));
	connect (ui->action_Add_Dummy_Spouse, SIGNAL(triggered()), this, SLOT(addNewDummySpouse()));
	connect (ui->action_Add_Male_Sibling, SIGNAL(triggered()), this, SLOT(addNewMaleSibling()));
	connect (ui->action_Add_Female_Sibling, SIGNAL(triggered()), this, SLOT(addNewFemaleSibling()));
	connect (ui->action_Add_Wife_in_Law, SIGNAL(triggered()), this, SLOT(addNewFemaleSibling()));
	connect (ui->action_Add_Male_Child, SIGNAL(triggered()), this, SLOT(addNewMaleChild()));
	connect (ui->action_Add_Female_Child, SIGNAL(triggered()), this, SLOT(addNewFemaleChild()));

	connect (ui->action_Add_Family, SIGNAL(triggered()), this, SLOT(addNewFamily()));

	connect (ui->action_Set_Document_Title, SIGNAL(triggered()), this, SLOT(setDocumentTitle()));

	connect (ui->action_Full_Text_Search, SIGNAL(triggered(bool)), this, SLOT(fullTextSearch(bool)));
	connect (ui->action_Preferences, SIGNAL(triggered()), this, SLOT(editPreferences()));
}

bool EditMenuHandler::addNewItem (Familier *fam, int row, QModelIndex &parent) const
{
	if (_treeModel->insertRow(fam, false, row, parent))
	{
		emit documentModified();

		QModelIndex index = _treeModel->index(row, 0, parent);
		TreeItem *item = _treeModel->getItem (index);
		switch (SettingsManager::s_LastCurrentView)
		{
		case SettingsManager::e_TreeView:
			{
				ui->treeView->setExpanded(parent, false);
				ui->treeView->setExpanded(parent, true);
				ui->treeView->setExpanded(index, false);
				ui->treeView->setExpanded(index, true);
				ui->treeView->scrollTo(index);
				if (!item->isDummy ())
					ui->treeView->edit(index);
			}
			break;
		case SettingsManager::e_FamilyTreeView:
			{
				QModelIndex parentInMappedModel = _familyTreeModel->mapFromSource (parent);
				ui->familyTreeView->setExpanded (parentInMappedModel, false);
				ui->familyTreeView->setExpanded (parentInMappedModel, true);
				QModelIndex indexInMappedModel = _familyTreeModel->mapFromSource (index);
				ui->familyTreeView->scrollTo (indexInMappedModel);
				if (!item->isDummy () &&
				    ui->familyTreeView->getShowBasicDetailsForItem ())
					ui->familyTreeView->edit (indexInMappedModel);
			}
		default:
			break;
		}

		return true;
	}

	return false;
}

bool EditMenuHandler::addNewSpouse(ItemType spouseType, bool isDummy)
{
	QModelIndex currentIndex = SettingsManager::currentIndexForCurrentViewForTreeModel ();
	if (!currentIndex.isValid())
		return false;

	TreeItem *item = _treeModel->getItem(currentIndex);

	Familier *fam = NULL;

	switch (spouseType)
	{
	case male:
		return false;
	case female:
		if (!item->isIndividual() || item->isFemale())
			return false;
		fam = new FemaleIndividual;
		break;
	case family:
		return false;
	}

	if (isDummy)
		dynamic_cast<Individual*>(fam)->setDummy (true);

	return addNewItem(fam, _treeModel->rowCount(currentIndex), currentIndex);
}

bool EditMenuHandler::addNewSibling(ItemType siblingType)
{
	QModelIndex currentIndex = SettingsManager::currentIndexForCurrentViewForTreeModel ();
	if (!currentIndex.isValid())
		return false;

	TreeItem *item = _treeModel->getItem(currentIndex);

	Familier *fam = NULL;

	switch (siblingType)
	{
	case male:
		if (!item->isIndividual() || item->isSpouseItem())
			return false;
		fam = new MaleIndividual;
		break;
	case female:
		if (!item->isIndividual())
			return false;
		fam = new FemaleIndividual;
		break;
	case family:
		return false;
		break;
	}

	QModelIndex index = _treeModel->parent(currentIndex);
	return addNewItem(fam, item->myChildNumber() + 1, index);
}

bool EditMenuHandler::addNewChild (ItemType childType)
{
	QModelIndex currentIndex = SettingsManager::currentIndexForCurrentViewForTreeModel ();
	if (!currentIndex.isValid())
		return false;

	TreeItem *item = _treeModel->getItem(currentIndex);

	Familier *fam = NULL;

	switch (childType)
	{
	case male:
		if (item->isIndividual() && !item->isFemale() && item->isSpouseItem())
			return false;
		fam = new MaleIndividual;
		break;
	case female:
		if (item->isIndividual() && !item->isFemale() && item->isSpouseItem())
			return false;
		fam = new FemaleIndividual;
		break;
	case family:
		return false;
		break;
	}

	return addNewItem(fam, 0, currentIndex);
}

bool EditMenuHandler::markAsDeadOrAlive(const bool dead)
{
	QModelIndex currentIndex = SettingsManager::currentIndexForCurrentViewForTreeModel ();
	if (!currentIndex.isValid())
		return false;

	TreeItem *item = _treeModel->getItem(currentIndex);
	if (!item->isIndividual())
		return false;

	QModelIndex index = _treeModel->index(currentIndex.row(), Familier::e_col_dead, currentIndex.parent());
	return _treeModel->setData(index, dead, Qt::EditRole);
}

bool EditMenuHandler::markOrUnmarkAsDummy(const bool dummy)
{
	QModelIndex currentIndex = SettingsManager::currentIndexForCurrentViewForTreeModel ();
	if (!currentIndex.isValid())
		return false;

	TreeItem *item = _treeModel->getItem(currentIndex);
	if (!item->isIndividual())
		return false;

	if (dummy)
	{
		if (QMessageBox::question(dynamic_cast<QWidget*>(parent()),
					  "Mark as dummy",
					  QString ("Are you sure you want to mark \"%1\" "
						   "as dummy?<br><br>This might result in "
						   "potential data loss, if you want to "
						   "return to non-dummy state!")
						.arg(item->fam()->getName()),
					  QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
			return false;
	}

	QModelIndex index = _treeModel->index(currentIndex.row(), Familier::e_col_dummyOrBase, currentIndex.parent());
	return _treeModel->setData(index, dummy, Qt::EditRole);
}

void EditMenuHandler::editInline ()
{
	QAbstractItemView *view = SettingsManager::currentView ();
	if (!view)
		return;

	QModelIndex currentIndex = view->currentIndex ();
	if (!currentIndex.isValid())
		return;

	view->edit(currentIndex);
}

void EditMenuHandler::markAsDead ()
{
	markAsDeadOrAlive(true);
}

void EditMenuHandler::markAsAlive()
{
	markAsDeadOrAlive(false);
}

void EditMenuHandler::markAsDummy()
{
	markOrUnmarkAsDummy(true);
}

void EditMenuHandler::unmarkAsDummy()
{
	markOrUnmarkAsDummy(false);
}

void EditMenuHandler::editEntry ()
{
	QModelIndex currentIndex = SettingsManager::currentIndexForCurrentViewForTreeModel ();

	editEntry(currentIndex);
}

void EditMenuHandler::editEntry (QModelIndex index)
{
	if (!index.isValid())
		return;

	TreeItem *item= _treeModel->getItem(index);

	EditDialog *dialog = NULL;
	if (item->isIndividual())
	{
		Individual *individual = dynamic_cast<Individual *>(item->fam());
		_eventModel->setIndividual (individual);
		dialog = new EditIndividualDialog(_treeModel, item, _iconModel, _familyModel,
						  _eventModel, NULL);
		connect (dynamic_cast <EditIndividualDialog*> (dialog), SIGNAL(imageEdited(IDTYPE)),
			 _iconModel, SLOT(regenerateModel()));
	}
	else if (item->parentItem())
		dialog = new EditFamilyDialog(_treeModel, item, NULL);

	bool editAgain = true;
	while (editAgain)
	{
		editAgain = false;
		dialog->show();
		if (QDialog::Accepted != dialog->exec())
		{
			if (dialog->isEdited())
			{
				if (QMessageBox::Ok != QMessageBox::warning(dialog, "Discard changes", "Discard changes made in this dialog box?", QMessageBox::Ok, QMessageBox::Cancel))
					editAgain = true;
			}
		}
		else if (dialog->save())
		{
			emit documentModified();
			emit itemViewRequiresUpdate();
		}
	}

	delete dialog;
	_eventModel->setIndividual (0);
}

void EditMenuHandler::delEntry ()
{
	QModelIndex currentIndex = SettingsManager::currentIndexForCurrentViewForTreeModel ();
	if (!currentIndex.isValid())
		return;

	TreeItem *item = _treeModel->getItem(currentIndex);
	Familier *fam = item->fam();
	if (QMessageBox::question(dynamic_cast<QWidget*>(parent()),
				  "Delete", QString ("Are you sure you want to delete \"%1\"?").arg(fam->getName()),
				  QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
		return;
	if (_treeModel->removeRow(item->myChildNumber(), _treeModel->parent(currentIndex)))
	{
		emit documentModified ();
		emit itemViewRequiresUpdate ();
	}
}

void EditMenuHandler::addNewFamily ()
{
	bool base = false;
	QMessageBox::StandardButton button = QMessageBox::question(
			dynamic_cast<QWidget*>(parent()),
			tr("Base Family?"), tr("Do you want the new family to be a base family?"
					       "<br><br>Base families are displayed<br>at the "
					       "top of the Tree View."),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
	if (button == QMessageBox::Cancel)
		return;

	if (button == QMessageBox::Yes)
		base = true;
	else
		base = false;

	Family *family = new Family;
	family->setBase(base);

	/* Family used to represent child items of root is also part
	   of the Maps so ignoring them
	 */
	QModelIndex rootIndex = QModelIndex();
	TreeItem *rootItem = _treeModel->getItem(rootIndex);
	Family *rootFamily = dynamic_cast<Family*>(rootItem->fam());
	int row = 0;
	if (base)
	{
		foreach(Family *temp, Family::s_NameToBaseFamilyMap)
		{
			if (temp == family)
				break;
			if (temp != rootFamily)
				row ++;
		}
	}
	else
	{
		row += Family::s_NameToBaseFamilyMap.count();

		foreach(Family *temp, Family::s_NameToNonBaseFamilyMap)
		{
			if (temp == family)
				break;
			if (temp != rootFamily)
				row ++;
		}
	}

	addNewItem(family, row, rootIndex);
}

void EditMenuHandler::addNewSpouse()
{
	addNewSpouse (female);
}

void EditMenuHandler::addNewDummySpouse ()
{
	addNewSpouse (female, true);
}

void EditMenuHandler::addNewMaleSibling()
{
	addNewSibling (male);
}

void EditMenuHandler::addNewFemaleSibling ()
{
	addNewSibling (female);
}

void EditMenuHandler::addNewMaleChild ()
{
	addNewChild (male);
}

void EditMenuHandler::addNewFemaleChild ()
{
	addNewChild (female);
}

void EditMenuHandler::setDocumentTitle()
{
	DocumentTitleDialog *dialog = new DocumentTitleDialog();

	dialog->ui->lineEditDocumentTitle->setText(SettingsManager::s_DocumentTitle);
	dialog->ui->labelDocumentCreationTimeValue->setText(SettingsManager::s_DocumentCreationDateTime.toString());
	dialog->ui->labelDocumentModificationTimeValue->setText(SettingsManager::s_DocumentModificationDateTime.toString());
	dialog->ui->lcdNumberDocumentModificationCount->display(SettingsManager::s_DocumentModificationCount);

	if (QDialog::Accepted == dialog->exec())
	{
		SettingsManager::s_DocumentTitle = dialog->ui->lineEditDocumentTitle->text();
		emit documentModified();
	}

	delete dialog;
}

void EditMenuHandler::fullTextSearch (const bool &searching) const
{
	if (!searching)
		return;

	bool ok;
	QString titleText ("Full Text Search");
	QString labelText ("Search String");
	QString defaultText;
	QString htmlNewLine ("<br>");
	QString searchString = QInputDialog::getText (
				dynamic_cast<QWidget*>(parent()), titleText,
				labelText, QLineEdit::Normal, defaultText , &ok);

	QStringList displayResults;
	bool found = false;
	if (!ok || searchString.isEmpty ())
	{
		ui->dockWidgetSearchResults->setHidden (true);
		return;
	}

	QRegExp searchRegExp (searchString);
	searchRegExp.setCaseSensitivity (Qt::CaseInsensitive);
	ui->textBrowserSearchResults->clear ();

	for (int i = 0; i < Family::s_FamilyList.size (); i++)
	{
		displayResults.clear ();

		Family *family = Family::s_FamilyList.at (i);
		if (!family->fullTextSearch (searchRegExp, displayResults))
			continue;

		found = true;
		ui->textBrowserSearchResults->append (QString("<b>Family:</b> %1<br>")
						 .arg (family->getAnchor (0, true, true)));
		ui->textBrowserSearchResults->append (
					displayResults.join (htmlNewLine).replace (
						QRegExp (QString ("(%1)").arg (searchString), Qt::CaseInsensitive),
						"<span style=\"background-color: yellow;\">\\1</span>"));
		ui->textBrowserSearchResults->append ("<hr>");
	}

	for (int i = 0; i < Individual::s_IndividualList.size (); i++)
	{
		displayResults.clear ();

		Individual *individual = Individual::s_IndividualList.at (i);
		bool individualFound = individual->fullTextSearch (searchRegExp, displayResults);

		bool eventFound = false;
		foreach (Event *event, Event::s_EventList)
		{
			if (!event->hasEventMember(individual->getId ()))
				continue;

			if (!event->fullTextSearch (searchRegExp, displayResults))
				continue;

			eventFound = true;
			found = true;
		}

		if (!individualFound && !eventFound)
			continue;

		found = true;
		ui->textBrowserSearchResults->append (QString("<b>Individual:</b> %1<br>")
						 .arg (individual->getAnchor (0, true, true)));
		ui->textBrowserSearchResults->append (
					displayResults.join (htmlNewLine).replace (
						QRegExp (QString ("(%1)").arg (searchString), Qt::CaseInsensitive),
						"<span style=\"background-color: yellow;\">\\1</span>"));
		ui->textBrowserSearchResults->append ("<hr>");
	}

	if (!found)
	{
		ui->textBrowserSearchResults->setHtml ("No results found!");
		return;
	}

	ui->textBrowserSearchResults->verticalScrollBar ()->setValue (0);
}

void EditMenuHandler::editPreferences ()
{
	MainPreferences *dialog = new MainPreferences (dynamic_cast<QWidget*>(parent()));

	dialog->ui->checkBoxDoNotRestoreLastSession->setChecked (SettingsManager::s_DoNotRestoreLastSession);
	dialog->ui->checkBoxLoadLastDocumentAutomatically->setChecked (SettingsManager::s_LoadLastDocumentAutomatically);
	dialog->ui->checkBoxExportForceLastName->setChecked (SettingsManager::s_ExportForceLastName);
	dialog->ui->checkBoxDisplayIndianRelations->setChecked (SettingsManager::s_DisplayIndianRelations);
	dialog->ui->checkBoxEnableSystemTray->setChecked (SettingsManager::s_EnableSystemTray);
	dialog->ui->checkBoxMimimizeMinimizesToSystemTray->setChecked (SettingsManager::s_MinimizeMinimizesToSystemTray);
	dialog->ui->checkBoxStartMinimized->setChecked (SettingsManager::s_StartupMinimized);
	dialog->ui->checkBoxStartLocked->setChecked (SettingsManager::s_StartupLocked);
	dialog->ui->checkBoxEnablePassword->setChecked (SettingsManager::s_EnablePassword);
	dialog->ui->lineEditPassword->setText (SettingsManager::s_Password);
	dialog->ui->checkBoxMinimizeLocksAutomatically->setChecked (SettingsManager::s_MinimizeLocksAutomatically);

	if (QDialog::Accepted == dialog->exec())
	{
		SettingsManager::s_DoNotRestoreLastSession = dialog->ui->checkBoxDoNotRestoreLastSession->isChecked ();
		SettingsManager::s_LoadLastDocumentAutomatically = dialog->ui->checkBoxLoadLastDocumentAutomatically->isChecked ();
		SettingsManager::s_ExportForceLastName = dialog->ui->checkBoxExportForceLastName->isChecked ();
		SettingsManager::s_DisplayIndianRelations = dialog->ui->checkBoxDisplayIndianRelations->isChecked ();
		SettingsManager::s_EnableSystemTray = dialog->ui->checkBoxEnableSystemTray->isChecked ();
		SettingsManager::s_MinimizeMinimizesToSystemTray = dialog->ui->checkBoxMimimizeMinimizesToSystemTray;
		SettingsManager::s_StartupMinimized = dialog->ui->checkBoxStartMinimized->isChecked ();
		SettingsManager::s_StartupLocked = dialog->ui->checkBoxStartLocked->isChecked ();
		SettingsManager::s_EnablePassword = dialog->ui->checkBoxEnablePassword->isChecked ();
		SettingsManager::s_Password = dialog->ui->lineEditPassword->text ();
		SettingsManager::s_MinimizeLocksAutomatically = dialog->ui->checkBoxMinimizeLocksAutomatically->isChecked ();
		emit systemTrayNeedsToBeShown (SettingsManager::s_EnableSystemTray);
	}

	delete dialog;
}
