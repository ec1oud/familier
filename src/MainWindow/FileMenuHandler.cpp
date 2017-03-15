#include "FileMenuHandler.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include "SettingsManager.h"
#include <QMessageBox>
#include "ActionWithIdentity.h"
#include <QDomDocument>
#include "Family.h"
#include "MaleIndividual.h"
#include "FemaleIndividual.h"
#include "Union.h"
#include "Event.h"

//#define DEBUGMODE

#ifdef DEBUGMODE
#define DEBUGITEMS(type) \
foreach (type *temp, type::s_ ## type ## List) \
	qDebug () << (*temp);
#endif

#define CLEARALLITEMS(type) \
	foreach (type *temp, type::s_ ## type ## List) \
		delete temp; \
	type::s_IdTo ## type ## Hash.clear(); \
	type::s_ ## type ## List.clear(); \
	type::s_Max ## type ## Id = 0;

FileMenuHandler::FileMenuHandler(EventModel *eventModel, Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui),
	_treeModel (dynamic_cast<TreeModel*>(ui->treeView->model ())),
	_tableModel (dynamic_cast<TableModel*>(ui->tableView->model ())),
	_iconModel (dynamic_cast<IconModel*>(ui->listView->model ())),
	_eventModel (eventModel)
{
	_documentModified = false;

	connect(ui->action_New, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui->action_Close, SIGNAL(triggered()), this, SLOT(closeFile()));
	connect(ui->action_Save, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(ui->action_Save_As, SIGNAL(triggered()), this, SLOT(saveFileAs()));
	connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(exit()));

	connect(ui->action_Export, SIGNAL(triggered()), this, SLOT(exportFile()));
	connect(ui->action_Import, SIGNAL(triggered()), this, SLOT(importFile()));

	connect (_treeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		 this, SLOT(setDocumentModified()));

	connect (_eventModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		 this, SLOT(setDocumentModified()));

	initializeRecentFileActions();
	updateRecentFileActions ();
}

bool FileMenuHandler::openFile (QString fileName)
{
	QDomDocument doc("familydocument");

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(dynamic_cast<QWidget*>(parent()), tr("Error reading file"), tr("Cannot read file %1:\n%2.")
				     .arg(fileName).arg(file.errorString()));
		return false;
	}
	QString err;
	int x, y;
	if (!doc.setContent((QIODevice *)&file, &err, &x, &y))
	{
		file.close();
		QMessageBox::warning(dynamic_cast<QWidget*>(parent()),
				     tr("Error reading file"),
				     tr("Invalid XML file %1.\n"
					"It contains the following error at line %3 and column %4:\n\"%2\"")
				     .arg(fileName).arg(err).arg(x).arg(y));
		return false;
	}
	file.close();

	QDomElement root = doc.documentElement();
	if(root.tagName() != "FamilierData"
	   || 1.0 != root.attribute("version", "0").trimmed().toDouble()
		|| !root.attribute("title", "").trimmed().length())
	{
		QMessageBox::warning(dynamic_cast<QWidget*>(parent()),
				     tr("Error Reading File"),
				     tr("Not a valid XML file.\n\n"
					"It doesn't contain FamiliesAndIndividuals tag\n"));
		return false;
	}

	SettingsManager::s_DocumentTitle = root.attribute("title", "").trimmed();

	QDomNodeList nlMetaData = root.elementsByTagName("DocumentMetaData");
	QDomNode nMetaData = nlMetaData.at(0);
	if (!nMetaData.isNull() && nMetaData.isElement())
	{
		QDomElement eMetaData = nMetaData.toElement();
		QString creationDateTime = eMetaData.attribute("creationdatetime", QDateTime::currentDateTime().toString());
		SettingsManager::s_DocumentCreationDateTime = QDateTime::fromString(creationDateTime);
		QString modificationDateTime = eMetaData.attribute("modificationdatetime", QDateTime::currentDateTime().toString());
		SettingsManager::s_DocumentModificationDateTime = QDateTime::fromString(modificationDateTime);
		SettingsManager::s_DocumentModificationCount = eMetaData.attribute("modificationcount", "0").trimmed().toULong();
	}

	QDomNodeList familyNodesList = root.elementsByTagName("Family");
	QDomNodeList individualNodesList = root.elementsByTagName("Individual");
	QDomNodeList unionNodesList = root.elementsByTagName("Union");
	QDomNodeList eventNodesList = root.elementsByTagName("Event");

	QProgressDialog progress(tr("Loading file"), tr("Interrupt"), 0,
				 3 * (familyNodesList.count() + individualNodesList.count() +
				 unionNodesList.count() + eventNodesList.count()),
				 dynamic_cast<QWidget*>(parent()));
	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(0);
	progress.setMinimumDuration(0);

	for (int i = 0; i < familyNodesList.count(); i++)
	{
		QDomNode familyNode = familyNodesList.at(i);
		new Family (familyNode.toElement());
		progress.setValue(progress.value() + 1);
	}

	for (int i = 0; i < individualNodesList.count(); i++)
	{
		QDomNode individualNode = individualNodesList.at(i);
		if (individualNode.toElement().attribute("type", "").trimmed() == "Female")
			new FemaleIndividual (individualNode.toElement());
		else
			new MaleIndividual (individualNode.toElement());
		progress.setValue(progress.value() + 1);
	}

	for (int i = 0; i < unionNodesList.count(); i++)
	{
		QDomNode unionNode = unionNodesList.at(i);
		new Union (unionNode.toElement());
		progress.setValue(progress.value() + 1);
	}

	for (int i = 0; i < eventNodesList.count(); i++)
	{
		QDomNode eventNode = eventNodesList.at(i);
		new Event (eventNode.toElement());
		progress.setValue(progress.value() + 1);
	}

	for (int i = 0; i < Union::s_UnionList.count(); i++)
	{
		QDomElement unionElement = unionNodesList.at(i).toElement();
		Union::s_UnionList.value(i)->setupItemLinks(unionElement);
		progress.setValue(progress.value() + 1);
	}

	for (int i = 0; i < eventNodesList.count(); i++)
	{
		QDomElement eventElement = eventNodesList.at(i).toElement();
		IDTYPE id = eventElement.attribute("id").trimmed().toULong();
		Event::s_IdToEventHash[id]->setupItemLinks(eventElement);
		progress.setValue(progress.value() + 1);
	}

	for (int i = 0; i < familyNodesList.count(); i++)
	{
		QDomElement familyElement = familyNodesList.at(i).toElement();
		IDTYPE id = familyElement.attribute("id").trimmed().toULong();
		Family::s_IdToFamilyHash[id]->setupItemLinks(familyElement);
		progress.setValue(progress.value() + 1);
	}

	for (int i = 0; i < individualNodesList.count(); i++)
	{
		QDomElement individualElement = individualNodesList.at(i).toElement();
		IDTYPE id = individualElement.attribute("id").trimmed().toULong();
		Individual::s_IdToIndividualHash[id]->setupItemLinks(individualElement);
		progress.setValue(progress.value() + 1);
	}

	foreach (Family *family, Family::s_FamilyList)
		family->updateStatistics ();

	SettingsManager::s_DocumentFilePath = fileName;
	SettingsManager::s_DocumentDirectoryPath = fileName.section('/', 0, -2);

	unsetDocumentModified();
	initialiseModel(progress);
	progress.close();

#ifdef DEBUGMODE
	qDebug () << "Families:" << Family::s_FamilyList.count ();
	qDebug () << "Individuals:" << Individual::s_IndividualList.count ();
	qDebug () << "Unions:" << Union::s_UnionList.count ();
	qDebug () << "Events:" << Event::s_EventList.count ();

	DEBUGITEMS(Family)
	DEBUGITEMS(Individual)
	DEBUGITEMS(Union)
	DEBUGITEMS(Event)
#endif

	SettingsManager::s_LastSaveDirectory = QFileInfo(fileName).absolutePath();
	if (SettingsManager::s_RecentFilesList.contains (fileName))
		SettingsManager::s_RecentFilesList.removeOne (fileName);
	SettingsManager::s_RecentFilesList.prepend (fileName);
	if (SettingsManager::s_RecentFilesList.size () > SettingsManager::s_MaxRecentFiles)
		SettingsManager::s_RecentFilesList.removeLast ();

	updateRecentFileActions ();

	return true;
}

bool FileMenuHandler::saveIfRequired ()
{
	return maybeSave ();
}

void FileMenuHandler::initializeRecentFileActions()
{
	for (int i = 0; i < SettingsManager::s_MaxRecentFiles; i++)
	{
		_recentFileActionsList.append(new ActionWithIdentity(i, this));
		_recentFileActionsList[i]->setVisible(false);
		connect (_recentFileActionsList[i], SIGNAL(triggered(unsigned long))
			 , this, SLOT(openRecentFile(unsigned long)));
		ui->menu_Recent_Files->addAction(_recentFileActionsList[i]);
	}
	_recentFileActionsList[0]->setShortcut(QKeySequence(tr("Ctrl+L")));
}

bool FileMenuHandler::closeFileWithoutSave()
{
	if (SettingsManager::s_DocumentFilePath.length())
	{
		SettingsManager::s_DocumentFilePath = "";
		SettingsManager::s_DocumentDirectoryPath = "";
		SettingsManager::s_DocumentTitle = "Untitled";
	}

	clearModel();

	CLEARALLITEMS(Union)
	CLEARALLITEMS(Event)
	CLEARALLITEMS(Individual)
	CLEARALLITEMS(Family)

	Family::s_NameToBaseFamilyMap.clear();
	Family::s_NameToNonBaseFamilyMap.clear();

	SettingsManager::s_CWD = QDir::currentPath();

	unsetDocumentModified();
	emit navigationItemsNeedClearing();
	emit itemViewNeedsClearing();

	return true;
}

bool FileMenuHandler::maybeSave()
{
	if (_documentModified)
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(dynamic_cast<QWidget*>(parent()), tr("Application"),
					   tr("The document has been modified.\n"
					      "Do you want to save your changes?"),
					   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save)
			return saveFile();
		else if (ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void FileMenuHandler::initialiseModel (QProgressDialog &progress)
{
	int row = 0;
	QModelIndex rootIndex;

	_tableModel->breakSignals ();
	_iconModel->breakSignals ();

	foreach (Family *family, Family::s_NameToBaseFamilyMap)
		family->setupModel(_treeModel, row++, rootIndex, progress);
	foreach (Family *family, Family::s_NameToNonBaseFamilyMap)
		family->setupModel(_treeModel, row++, rootIndex, progress);

	_tableModel->regenerateModel ();
	_iconModel->regenerateModel ();
	_eventModel->regenerateModel ();

	_tableModel->setupSignals ();
	_iconModel->setupSignals ();

	SettingsManager::t_LastCurrentView currentView = SettingsManager::s_LastCurrentView;
	SettingsManager::s_LastCurrentView = SettingsManager::e_TreeView;
	emit modelLoadingFinished();
	SettingsManager::s_LastCurrentView = SettingsManager::e_FamilyTreeView;
	emit modelLoadingFinished();
	SettingsManager::s_LastCurrentView = currentView;
}

void FileMenuHandler::clearModel()
{
	disconnect(_treeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		   this, SLOT(setDocumentModified()));
	_tableModel->clear ();
	_iconModel->clear ();
	_treeModel->clear();
	_eventModel->clear ();
	connect (_treeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		 this, SLOT(setDocumentModified()));
}

void FileMenuHandler::unsetDocumentModified()
{
	_documentModified = false;
	ui->action_Save->setDisabled(true);
	emit windowTitleRequiresChange(SettingsManager::s_FamilierApplicationTitle + " - " +
				       SettingsManager::s_DocumentTitle);
	emit statusBarNeedsUpdation();
}

void FileMenuHandler::updateRecentFileActions()
{
	QStringList files = SettingsManager::s_RecentFilesList;

	int numRecentFiles = qMin(files.size(), SettingsManager::s_MaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i)
	{
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
		_recentFileActionsList[i]->setText(text);
		_recentFileActionsList[i]->setData(files[i]);
		_recentFileActionsList[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < SettingsManager::s_MaxRecentFiles; ++j)
		_recentFileActionsList[j]->setVisible(false);
}

bool FileMenuHandler::newFile()
{
	if (maybeSave())
		return closeFileWithoutSave();

	return true;
}

bool FileMenuHandler::openFile()
{
	if (maybeSave())
	{
		closeFileWithoutSave();
		QString fileName = QFileDialog::getOpenFileName(dynamic_cast<QWidget*>(parent()),
								tr ("Select file to Open"),
								SettingsManager::s_LastSaveDirectory,
								tr("Familier Files (*.xml);;All Files (*.*)"));
		if (!fileName.isEmpty())
			return openFile(fileName);
		else
			return false;
	}

	return true;
}

bool FileMenuHandler::openRecentFile(const unsigned long recentFileNo)
{
	if (maybeSave())
	{
		closeFileWithoutSave();

		if ((int)recentFileNo >= SettingsManager::s_RecentFilesList.size())
			return false;

		QString fileName = SettingsManager::s_RecentFilesList.value (recentFileNo);
		if (!fileName.isEmpty())
			return openFile(fileName);
		else
			return false;
	}
	return true;
}

bool FileMenuHandler::closeFile()
{
	if (maybeSave())
		return closeFileWithoutSave();

	return true;
}

bool FileMenuHandler::saveFile()
{
	QString fileName = SettingsManager::s_DocumentFilePath;

	if (!fileName.length())
		return saveFileAs();

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
	{
		QMessageBox::warning(dynamic_cast<QWidget*>(parent()),
				     tr("Error writing file"),
				     tr("Cannot write to file %1:\n%2.")
				     .arg(fileName)
				     .arg(file.errorString()));
		return false;
	}

	QTextStream ts (&file);

	ts << "<?xml version=\"1.0\"?>" << endl;
	ts << "<FamilierData version=\"1.0\" title=\""
			<< SettingsManager::s_DocumentTitle
			<< "\">" << endl;
	ts << "\t<DocumentMetaData"
			<< " creationdatetime=\"" << SettingsManager::s_DocumentCreationDateTime.toString()
			<< "\" modificationdatetime=\"" << SettingsManager::s_DocumentModificationDateTime.toString()
			<< "\" modificationcount=\"" << SettingsManager::s_DocumentModificationCount
			<< "\" />" << endl;

	foreach (Family *family, Family::s_FamilyList)
	{
		Family temp = *family;
		ts << temp;
	}
	foreach (Individual *individual, Individual::s_IndividualList)
	{
		if (individual->isFemale())
		{
			FemaleIndividual temp = *(dynamic_cast<FemaleIndividual*>(individual));
			ts << temp;
		}
		else
		{
			MaleIndividual temp = *(dynamic_cast<MaleIndividual*>(individual));
			ts << temp;
		}
	}
	foreach (Union *aUnion, Union::s_UnionList)
	{
		Union temp = *aUnion;
		ts << temp;
	}
	foreach (Event *event, Event::s_EventList)
	{
		Event temp = *event;
		ts << temp;
	}

	ts << "</FamilierData>" << endl;

	file.close();

	_documentModified = false;
	emit windowTitleRequiresChange(SettingsManager::s_DocumentTitle);

	ui->action_Save->setDisabled(true);

	SettingsManager::s_LastSaveDirectory = QFileInfo(fileName).absolutePath();

	return true;
}

bool FileMenuHandler::saveFileAs()
{
	QString fileName = QFileDialog::getSaveFileName(dynamic_cast<QWidget*>(parent()),
							tr ("Select file to Save"),
							SettingsManager::s_DocumentDirectoryPath,
							tr("Familier Files (*.xml);;All Files (*.*)"));
	if (fileName.isEmpty())
		return false;

	SettingsManager::s_DocumentFilePath = fileName;

	return saveFile();
}

bool FileMenuHandler::exit()
{
	if (maybeSave())
	{
		emit exitFromApplication();
		return true;
	}

	return false;
}

bool FileMenuHandler::importFile()
{
	if (maybeSave())
	{
		closeFileWithoutSave();

		QString fileName = QFileDialog::getOpenFileName (dynamic_cast<QWidget*>(parent ()),
								 tr ("Select file to Open"),
								 SettingsManager::s_LastExportDirectory,
								 tr ("GEDCOM Files (*.ged);;All Files (*.*)"));

		if (fileName.isEmpty ())
			return false;

		QFile file (fileName);
		if (!file.open (QIODevice::ReadOnly))
		{
			QMessageBox::warning (dynamic_cast<QWidget*>(parent()),
					      tr("Error Reading file"),
					      tr("Cannot open file for writing %1:\n%2.")
					      .arg (fileName).arg (file.errorString()));
			return false;
		}

		QTextStream ts (&file);

		enum { none, head, individual, family, note } levelTag;
		QStringList headerList;
		QHash <QString, QStringList> xRefToIndividualGEDCOMInfoHash;
		QHash <QString, QStringList> xRefToUnionGEDCOMInfoHash;
		QHash <QString, QStringList> xRefToNoteGEDCOMInfoHash;
		QHash<QString, Union*> xRefToUnionHash;
		QHash<QString, Individual*> xRefToIndividualHash;


		QString xRef;
		levelTag = none;
		while (!ts.atEnd ())
		{
			QString line = ts.readLine ().trimmed ();
			QList<QString> list = line.split (QRegExp ("\\s+"));
			if (list.count () <= 1)
				continue;

			int level = list.first ().toInt ();
			if (level)
			{
				switch (levelTag)
				{
				case head:
					headerList << line;
					break;
				case individual:
					if (list.at (1) == "SEX")
					{
						if (list.size () != 3)
							continue;

						Individual *individual;
						if (list.at (2) == "F")
							individual = new FemaleIndividual;
						else
							individual = new MaleIndividual;
						xRefToIndividualHash [xRef] = individual;
					}
					xRefToIndividualGEDCOMInfoHash [xRef] << line;
					break;
				case family:
					xRefToUnionGEDCOMInfoHash [xRef] << line;
					break;
				case note:
					xRefToNoteGEDCOMInfoHash [xRef] << line;
					break;
				default:
					break;
				}
			}
			else
			{
				QString tag = list.last ();
				if (tag == "HEAD")
				{
					if (headerList.isEmpty ())
						levelTag = head;
					else
						levelTag = none;
				}
				else if (tag == "TRLR")
					break;
				else if (tag == "INDI")
				{
					xRef = list.at (1);
					if (xRef.isEmpty ())
						levelTag = none;
					else if (!xRefToIndividualGEDCOMInfoHash.contains (xRef))
						levelTag = individual;
					else
					{
						levelTag = none;
						qDebug () << "Duplicate entry for Individual:" << xRef;
					}
				}
				else if (tag == "FAM")
				{
					xRef = list.at (1);
					if (xRef.isEmpty ())
						levelTag = none;
					else if (!xRefToUnionGEDCOMInfoHash.contains (xRef))
					{
						levelTag = family;
						Union *aUnion = new Union;
						xRefToUnionHash [xRef] = aUnion;
					}
					else
					{
						levelTag = none;
						qDebug () << "Duplicate entry for Family:" << xRef;
					}
				}
				else if (tag == "NOTE")
				{
					xRef = list.at (1);
					if (xRef.isEmpty ())
						levelTag = none;
					else  if (!xRefToNoteGEDCOMInfoHash.contains (xRef))
					{
						levelTag = note;
						if (list.size () > 3)
						{
							QString line = "";
							for (int i = 2; i < list.size (); i++)
								line += list.at (i);
							xRefToNoteGEDCOMInfoHash [xRef] << "1 CONT" << line;
						}
					}
					else
					{
						levelTag = none;
						qDebug () << "Duplicate entry for Note:" << xRef;
					}
				}
				else
					levelTag = none;
			}
		}

		QProgressDialog progress(tr("Loading file"), tr("Interrupt"), 0,
					 6 * (xRefToIndividualHash.count () + xRefToUnionHash.count ()),
					 dynamic_cast<QWidget*>(parent()));
		progress.setWindowModality(Qt::WindowModal);
		progress.setCancelButton(0);
		progress.setMinimumDuration(0);

		for (QHash<QString, Individual*>::const_iterator i = xRefToIndividualHash.constBegin ();
			i != xRefToIndividualHash.constEnd (); i++)
		{
			QString xRef = i.key ();
			Individual *individual = i.value ();
			const QStringList &list = xRefToIndividualGEDCOMInfoHash.value (xRef);
			individual->fromGEDCOM (list, xRefToUnionHash, xRefToNoteGEDCOMInfoHash);
			progress.setValue(progress.value() + 1);
		}

		for (QHash<QString, Union*>::const_iterator i = xRefToUnionHash.constBegin ();
			i != xRefToUnionHash.constEnd (); i++)
		{
			QString xRef = i.key ();
			Union *aUnion = i.value ();
			const QStringList &list = xRefToUnionGEDCOMInfoHash.value (xRef);
			aUnion->fromGEDCOM (list, xRefToIndividualHash, xRefToNoteGEDCOMInfoHash);
			progress.setValue(progress.value() + 1);
		}

#ifdef DEBUGMODE
		qDebug () << "Families:" << Family::s_FamilyList.count ();
		qDebug () << "Individuals:" << Individual::s_IndividualList.count ();
		qDebug () << "Unions:" << Union::s_UnionList.count ();
		qDebug () << "Events:" << Event::s_EventList.count ();

		DEBUGITEMS(Family)
		DEBUGITEMS(Individual)
		DEBUGITEMS(Union)
		DEBUGITEMS(Event)
#endif

		foreach (Individual *individual, Individual::s_IndividualList)
		{
			if (!individual->getFamily ())
			{
				Individual *temp = individual->getFather ();
				while (temp)
				{
					individual->setFamily (temp->getFamily ());
					temp = temp->getFather ();
				}
			}
			progress.setValue (progress.value () + 1);
		}

		foreach (Family *family, Family::s_FamilyList)
		{
			family->updateStatistics ();
			progress.setValue (progress.value () + 1);
		}

		initialiseModel(progress);
		progress.close();
		setDocumentModified ();
	}

	return true;
}

bool FileMenuHandler::exportFile()
{
	QString fileName = QFileDialog::getSaveFileName(dynamic_cast<QWidget*>(parent()),
							tr ("Select file to Save"),
							SettingsManager::s_LastExportDirectory,
							tr("GEDCOM Files (*.ged);;All Files (*.*)"));
	if (fileName.isEmpty())
		return false;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
	{
		QMessageBox::warning(dynamic_cast<QWidget*>(parent()),
				     tr("Error writing file"),
				     tr("Cannot open file for writing %1:\n%2.")
				     .arg(fileName).arg(file.errorString()));
		return false;
	}

	QTextStream ts (&file);

	ts << "0 HEAD" << endl;
	ts << "1 SOUR " << SettingsManager::s_FamilierApplicationName << endl;
	ts << "2 VERS " << SettingsManager::s_FamilierVersion << endl;
	ts << "2 NAME " << SettingsManager::s_FamilierApplicationName << endl;
	ts << "1 DATE " << SettingsManager::s_DocumentModificationDateTime.toString("dd MMM yyyy") << endl;
	ts << "2 TIME " << SettingsManager::s_DocumentModificationDateTime.toString("hh:mm:ss") << endl;
	ts << "1 CHAR ANSI" << endl;
	ts << "1 FILE " << fileName << endl;
	ts << "1 GEDC" << endl;
	ts << "2 VERS 5.5" << endl;
	ts << "2 FORM LINEAGE-LINKED" << endl;

	foreach (Individual * individual, Individual::s_IdToIndividualHash)
		individual->toGEDCOM(ts);
	foreach (Union *aUnion, Union::s_IdToUnionHash)
		aUnion->toGEDCOM(ts);

	ts << "0 TRLR" << endl;

	file.close();

	SettingsManager::s_LastExportDirectory = QFileInfo(fileName).absolutePath();

	return true;
}

void FileMenuHandler::setDocumentModified ()
{
	if (!_documentModified)
	{
		_documentModified = true;
		ui->action_Save->setEnabled (true);

		/* We will increment document modification count only while saving.
		   That is to say only once during one file save call.
		 */
		SettingsManager::s_DocumentModificationCount++;

		emit windowTitleRequiresChange(SettingsManager::s_FamilierApplicationTitle + " - " +
					       SettingsManager::s_DocumentTitle + " *");
	}

	SettingsManager::s_DocumentModificationDateTime = QDateTime::currentDateTime();
	emit statusBarNeedsUpdation();
}
