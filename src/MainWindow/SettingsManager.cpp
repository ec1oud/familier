#include "SettingsManager.h"
#include "versiondefs.h"
#include <QDir>
#include <QScrollBar>
#include <QSessionManager>
#include <QMessageBox>

#define STORE_SETTING_VALUE(settingname,settingvalue) \
	settings.setValue (s_EnumToPropertyNameHash.value (e_ ## settingname, "XXX"), \
	settingvalue);
#define STORE_SETTING_SESSION(settingname,settingvalue) STORE_SETTING_VALUE(Session ## settingname,settingvalue)
#define STORE_SETTING(settingname) STORE_SETTING_VALUE(settingname,s_ ## settingname)

#define RESTORE_SETTING_VALUE(settingname,defaultvalue,type) \
	settings.value (s_EnumToPropertyNameHash.value (e_ ## settingname, "XXX"), \
	defaultvalue).to ##type ()
#define RESTORE_SETTING_SESSION(settingname,defaultvalue,type) \
	RESTORE_SETTING_VALUE(Session ## settingname,defaultvalue,type)
#define RESTORE_SETTING(settingname,defaultvalue,type) \
	s_ ## settingname = RESTORE_SETTING_VALUE(settingname,defaultvalue,type);

QHash <long, QString> SettingsManager::s_EnumToPropertyNameHash;

Ui::MainWindow*	SettingsManager::ui;

QString		SettingsManager::s_DocumentTitle = "Untitled";
QDateTime	SettingsManager::s_DocumentCreationDateTime;
QDateTime	SettingsManager::s_DocumentModificationDateTime;
int		SettingsManager::s_DocumentModificationCount = 0;
QString		SettingsManager::s_DocumentFilePath;
QString		SettingsManager::s_DocumentDirectoryPath;
QString		SettingsManager::s_ProfileImageDirectory;

QString		SettingsManager::s_FamilierApplicationName = FAMILIERAPP;
QString		SettingsManager::s_FamilierOrganisation = FAMILIERORG;
QString		SettingsManager::s_FamilierDomain = FAMILIERDOMAIN;
QString		SettingsManager::s_FamilierAuthor = "Neeraj Jakhar";
QString		SettingsManager::s_FamilierAuthorEmail = "neeraj.jakhar@gmail.com";
const int	SettingsManager::s_FamilierMajorVersion = VER_MAJ;
const int	SettingsManager::s_FamilierMinorVersion = VER_MIN;
const int	SettingsManager::s_FamilierPatchVersion = VER_PAT;
const int	SettingsManager::s_FamilierBuildVersion = VER_BLD;
QString		SettingsManager::s_FamilierVersion = QString ("%1.%2.%3.%4")
						     .arg(SettingsManager::s_FamilierMajorVersion)
						     .arg(SettingsManager::s_FamilierMinorVersion)
						     .arg(SettingsManager::s_FamilierPatchVersion)
						     .arg(SettingsManager::s_FamilierBuildVersion);
QString		SettingsManager::s_FamilierApplicationTitle = SettingsManager::s_FamilierApplicationName +
							      " - " + SettingsManager::s_FamilierVersion;

int		SettingsManager::s_MaxRecentFiles = 4;
int		SettingsManager::s_MaxPieViewEntries = 15;
int		SettingsManager::s_MaxEditDialogImageWidth = 200;
QStringList	SettingsManager::s_RecentFilesList;
QString		SettingsManager::s_LastSaveDirectory;
QString		SettingsManager::s_LastExportDirectory;
QString		SettingsManager::s_CWD;
bool		SettingsManager::s_MainWindowIsLocked = false;
bool		SettingsManager::s_MainWindowIsVisible = false;

bool		SettingsManager::s_DoNotRestoreLastSession = false;
bool		SettingsManager::s_LoadLastDocumentAutomatically = true;
bool		SettingsManager::s_ExportForceLastName = false;
bool		SettingsManager::s_DisplayIndianRelations = false;
bool		SettingsManager::s_EnableSystemTray = false;
bool		SettingsManager::s_MinimizeMinimizesToSystemTray = false;
bool		SettingsManager::s_StartupMinimized = false;
bool		SettingsManager::s_StartupLocked = false;
bool		SettingsManager::s_EnablePassword = false;
QString		SettingsManager::s_Password;
bool		SettingsManager::s_MinimizeLocksAutomatically = false;

bool SettingsManager::s_FamilyTreeShowBasicItemDetails = true;
bool SettingsManager::s_FamilyTreeShowExtraItemDetails = true;
bool SettingsManager::s_IconViewShowIndividualsWOImages = true;
SettingsManager::t_LastCurrentView SettingsManager::s_LastCurrentView = SettingsManager::e_FamilyTreeView;

SettingsManager::SettingsManager (QObject *parent)
	: QObject(parent)
{
	QSettings settings (s_FamilierOrganisation, s_FamilierApplicationName);
	int view = settings.value (s_EnumToPropertyNameHash.value (e_LastCurrentView)).toInt ();
	if (view <= e_MinimumView || view >= e_MaximumView)
		s_LastCurrentView = e_FamilyTreeView;
	else
		s_LastCurrentView = (t_LastCurrentView)view;

	RESTORE_SETTING (LastSaveDirectory, QDir::currentPath (), String);
	RESTORE_SETTING (LastExportDirectory, QDir::currentPath (), String);
	RESTORE_SETTING (RecentFilesList, QStringList (), StringList);
	RESTORE_SETTING (ProfileImageDirectory, QDir::currentPath (), String);

	RESTORE_SETTING (FamilyTreeShowBasicItemDetails, true, Bool);
	RESTORE_SETTING (FamilyTreeShowExtraItemDetails, true, Bool);
	RESTORE_SETTING (IconViewShowIndividualsWOImages, true, Bool);

	RESTORE_SETTING (DoNotRestoreLastSession, false, Bool);
	RESTORE_SETTING (LoadLastDocumentAutomatically, true, Bool);
	RESTORE_SETTING (ExportForceLastName, false, Bool);
	RESTORE_SETTING (DisplayIndianRelations, false, Bool);
	RESTORE_SETTING (EnableSystemTray, false, Bool);
	RESTORE_SETTING (MinimizeMinimizesToSystemTray, false, Bool);
	RESTORE_SETTING (StartupMinimized, false, Bool);
	RESTORE_SETTING (StartupLocked, false, Bool);
	RESTORE_SETTING (EnablePassword, false, Bool);
	RESTORE_SETTING (Password, "", String);
	RESTORE_SETTING (MinimizeLocksAutomatically, false, Bool);

	s_MainWindowIsVisible = s_StartupMinimized;
}

SettingsManager::~SettingsManager ()
{
	QSettings settings (s_FamilierOrganisation, s_FamilierApplicationName);
	if (s_LastCurrentView)
		STORE_SETTING (LastCurrentView);
	if (s_LastSaveDirectory.length ())
		STORE_SETTING (LastSaveDirectory);
	if (s_LastExportDirectory.length ())
		STORE_SETTING (LastExportDirectory);
	if (s_RecentFilesList.size ())
		STORE_SETTING (RecentFilesList);
	if (s_ProfileImageDirectory.size ())
		STORE_SETTING (ProfileImageDirectory);

	s_FamilyTreeShowBasicItemDetails = ui->familyTreeView->getShowBasicDetailsForItem ();
	s_FamilyTreeShowExtraItemDetails = ui->familyTreeView->getShowExtraDetailsForItem ();
	STORE_SETTING (FamilyTreeShowBasicItemDetails);
	STORE_SETTING (FamilyTreeShowExtraItemDetails);
	STORE_SETTING (IconViewShowIndividualsWOImages);

	STORE_SETTING (DoNotRestoreLastSession)
	STORE_SETTING (LoadLastDocumentAutomatically)
	STORE_SETTING (ExportForceLastName)
	STORE_SETTING (DisplayIndianRelations)
	STORE_SETTING (EnableSystemTray)
	STORE_SETTING (MinimizeMinimizesToSystemTray)
	STORE_SETTING (StartupMinimized)
	STORE_SETTING (StartupLocked);
	STORE_SETTING (EnablePassword);
	STORE_SETTING (Password);
	STORE_SETTING (MinimizeLocksAutomatically);
}

void SettingsManager::initialize (Ui::MainWindow *_ui)
{
	ui = _ui;

	s_EnumToPropertyNameHash [e_LastCurrentView] = "LastCurrentView";
	s_EnumToPropertyNameHash [e_LastSaveDirectory] = "LastSaveDirectory";
	s_EnumToPropertyNameHash [e_LastExportDirectory] = "LastExportDirectory";
	s_EnumToPropertyNameHash [e_RecentFilesList] = "RecentFilesList";
	s_EnumToPropertyNameHash [e_ProfileImageDirectory] = "ProfileImageDirectory";

	s_EnumToPropertyNameHash [e_FamilyTreeShowBasicItemDetails] = "FamilyTreeShowBasicItemDetails";
	s_EnumToPropertyNameHash [e_FamilyTreeShowExtraItemDetails] = "FamilyTreeShowExtraItemDetails";
	s_EnumToPropertyNameHash [e_IconViewShowIndividualsWOImages] = "IconViewShowIndividualsWOImages";

	s_EnumToPropertyNameHash [e_DoNotRestoreLastSession] = "DoNotRestoreSession";
	s_EnumToPropertyNameHash [e_LoadLastDocumentAutomatically] = "LoadLastDocumentAutomatically";
	s_EnumToPropertyNameHash [e_ExportForceLastName] = "ExportForceLastName";
	s_EnumToPropertyNameHash [e_DisplayIndianRelations] = "DisplayIndianRelations";
	s_EnumToPropertyNameHash [e_EnableSystemTray] = "EnableSystemTray";
	s_EnumToPropertyNameHash [e_MinimizeMinimizesToSystemTray] = "MinimizeMinimizesToSystemTray";
	s_EnumToPropertyNameHash [e_StartupMinimized] = "StartupMinimized";
	s_EnumToPropertyNameHash [e_StartupLocked] = "StartupLocked";
	s_EnumToPropertyNameHash [e_EnablePassword] = "EnablePassword";
	s_EnumToPropertyNameHash [e_Password] = "Password";
	s_EnumToPropertyNameHash [e_MinimizeLocksAutomatically] = "MinimizeLocksAutomatically";

	s_EnumToPropertyNameHash [e_SessionOpenFile] = "OpenFile";
	s_EnumToPropertyNameHash [e_SessionTabIndex] = "TabIndex";

	s_EnumToPropertyNameHash [e_SessionFamilyTreeViewBasicDetails] = "FamilyTreeViewBasicDetails";
	s_EnumToPropertyNameHash [e_SessionFamilyTreeViewExtraDetails] = "FamilyTreeViewExtraDetails";
	s_EnumToPropertyNameHash [e_SessionFamilyTreeViewHorizontalScrollBar] = "FamilyTreeViewHorizontalScrollBar";
	s_EnumToPropertyNameHash [e_SessionFamilyTreeViewVerticalScrollBar] = "FamilyTreeViewVerticalScrollBar";

	s_EnumToPropertyNameHash [e_SessionEventTimeLineViewHorizontalScrollBar] = "EventTimeLineViewHorizontalScrollBar";
	s_EnumToPropertyNameHash [e_SessionEventTimeLineViewVerticalScrollBar] = "EventTimeLineViewVerticalScrollBar";

	s_EnumToPropertyNameHash [e_SessionStatisticsViewHorizontalScrollBar] = "StatisticsViewHorizontalScrollBar";
	s_EnumToPropertyNameHash [e_SessionStatisticsViewVerticalScrollBar] = "StatisticsViewVerticalScrollBar";

	s_EnumToPropertyNameHash [e_SessionStackedWidgetIndex] = "StackedWidgetIndex";

	s_EnumToPropertyNameHash [e_SessionTreeViewHorizontalScrollBar] = "TreeViewHorizontalScrollBar";
	s_EnumToPropertyNameHash [e_SessionTreeViewVerticalScrollBar] = "TreeViewVerticalScrollBar";

	s_EnumToPropertyNameHash [e_SessionTableViewHorizontalScrollBar] = "TableViewHorizontalScrollBar";
	s_EnumToPropertyNameHash [e_SessionTableViewVerticalScrollBar] = "TableViewVerticalScrollBar";

	s_EnumToPropertyNameHash [e_SessionListViewHorizontalScrollBar] = "ListViewHorizontalScrollBar";
	s_EnumToPropertyNameHash [e_SessionListViewVerticalScrollBar] = "ListViewVerticalScrollBar";

	s_EnumToPropertyNameHash [e_SessionTextBrowseItemViewHorizontalScrollBar] = "TextBrowseItemViewHorizontalScrollBar";
	s_EnumToPropertyNameHash [e_SessionTextBrowseItemViewVerticalScrollBar] = "TextBrowseItemViewVerticalScrollBar";
	s_EnumToPropertyNameHash [e_SessionTextBrowserLastOpenItems] = "TextBrowserLastOpenItems";

	s_EnumToPropertyNameHash [e_SessionTableViewForEventsHorizontalScrollBar] = "TableViewForEventsHorizontalScrollBar";
	s_EnumToPropertyNameHash [e_SessionTableViewForEventsVerticalScrollBar] = "TableViewForEventsVerticalScrollBar";
}

void SettingsManager::saveState (QSessionManager &sm, const QStringList &urlsList)
{
	QString filePath = sessionFilePath ();

	QStringList discardCommand;
	discardCommand << "rm" << filePath;
	sm.setDiscardCommand (discardCommand);

	QSettings settings (filePath, QSettings::IniFormat);
	saveState (settings, urlsList);
}

void SettingsManager::saveState (QSettings &settings, const QStringList &urlsList)
{
	STORE_SETTING_SESSION (OpenFile, s_DocumentFilePath);
	STORE_SETTING_SESSION (TabIndex, ui->tabWidget->currentIndex ());

	STORE_SETTING_SESSION (FamilyTreeViewBasicDetails, ui->familyTreeView->getShowBasicDetailsForItem ());
	STORE_SETTING_SESSION (FamilyTreeViewExtraDetails, ui->familyTreeView->getShowExtraDetailsForItem ());
	STORE_SETTING_SESSION (FamilyTreeViewHorizontalScrollBar, ui->familyTreeView->horizontalScrollBar ()->value ());
	STORE_SETTING_SESSION (FamilyTreeViewVerticalScrollBar, ui->familyTreeView->verticalScrollBar ()->value ());

	STORE_SETTING_SESSION (EventTimeLineViewHorizontalScrollBar, ui->eventTimeLineView->horizontalScrollBar ()->value ());
	STORE_SETTING_SESSION (EventTimeLineViewVerticalScrollBar, ui->eventTimeLineView->verticalScrollBar ()->value ());

	STORE_SETTING_SESSION (StatisticsViewHorizontalScrollBar, ui->scrollAreaStatisticsView->horizontalScrollBar ()->value ());
	STORE_SETTING_SESSION (StatisticsViewVerticalScrollBar, ui->scrollAreaStatisticsView->verticalScrollBar ()->value ());

	STORE_SETTING_SESSION (StackedWidgetIndex, ui->stackedWidget->currentIndex ());

	STORE_SETTING_SESSION (TreeViewHorizontalScrollBar, ui->treeView->horizontalScrollBar ()->value ());
	STORE_SETTING_SESSION (TreeViewVerticalScrollBar, ui->treeView->verticalScrollBar ()->value ());

	STORE_SETTING_SESSION (TableViewHorizontalScrollBar, ui->tableView->horizontalScrollBar ()->value ());
	STORE_SETTING_SESSION (TableViewVerticalScrollBar, ui->tableView->verticalScrollBar ()->value ());

	STORE_SETTING_SESSION (ListViewHorizontalScrollBar, ui->listView->horizontalScrollBar ()->value ());
	STORE_SETTING_SESSION (ListViewVerticalScrollBar, ui->listView->verticalScrollBar ()->value ());

	STORE_SETTING_SESSION (TextBrowseItemViewHorizontalScrollBar, ui->textBrowserItemView->horizontalScrollBar ()->value ());
	STORE_SETTING_SESSION (TextBrowseItemViewVerticalScrollBar, ui->textBrowserItemView->verticalScrollBar ()->value ());
	STORE_SETTING_SESSION (TextBrowserLastOpenItems, urlsList);

	STORE_SETTING_SESSION (TableViewForEventsHorizontalScrollBar, ui->tableViewForEvents->horizontalScrollBar ()->value ());
	STORE_SETTING_SESSION (TableViewForEventsVerticalScrollBar, ui->tableViewForEvents->verticalScrollBar ()->value ());
}

void SettingsManager::restoreState (FileMenuHandler *fmh, ItemViewManager *ivm)
{
	QString sFilePath = sessionFilePath ();

	QSettings settings (sFilePath, QSettings::IniFormat);

	QString filePath = RESTORE_SETTING_SESSION (OpenFile, "", String);
	if (!fmh->openFile (filePath))
		return;

	QStringList urlsList = RESTORE_SETTING_SESSION (TextBrowserLastOpenItems, QStringList(), StringList);
	foreach (QString url, urlsList)
		ivm->handleAnchorClick (url);

	ui->tabWidget->setCurrentIndex (RESTORE_SETTING_SESSION (TabIndex, 0, Int));
	ui->stackedWidget->setCurrentIndex (RESTORE_SETTING_SESSION (StackedWidgetIndex, 0, Int));

	ui->familyTreeView->setShowBasicDetailsForItem (RESTORE_SETTING_SESSION (FamilyTreeViewBasicDetails, true, Bool));
	ui->familyTreeView->setShowExtraDetailsForItem (RESTORE_SETTING_SESSION (FamilyTreeViewExtraDetails, true, Bool));
	ui->familyTreeView->horizontalScrollBar ()->setValue (RESTORE_SETTING_SESSION (FamilyTreeViewHorizontalScrollBar, 0, Int));
	ui->familyTreeView->verticalScrollBar ()->setValue (RESTORE_SETTING_SESSION (FamilyTreeViewVerticalScrollBar, 0, Int));

	ui->eventTimeLineView->horizontalScrollBar ()->setValue (RESTORE_SETTING_SESSION (EventTimeLineViewHorizontalScrollBar, 0, Int));
	ui->eventTimeLineView->verticalScrollBar ()->setValue (RESTORE_SETTING_SESSION (EventTimeLineViewVerticalScrollBar, 0, Int));

	ui->scrollAreaStatisticsView->horizontalScrollBar ()->setValue (RESTORE_SETTING_SESSION (StatisticsViewHorizontalScrollBar, 0, Int));
	ui->scrollAreaStatisticsView->verticalScrollBar ()->setValue (RESTORE_SETTING_SESSION (StatisticsViewVerticalScrollBar, 0, Int));

	ui->treeView->horizontalScrollBar ()->setValue (RESTORE_SETTING_SESSION (TreeViewHorizontalScrollBar, 0, Int));
	ui->treeView->verticalScrollBar ()->setValue (RESTORE_SETTING_SESSION (TreeViewVerticalScrollBar, 0, Int));

	ui->tableView->horizontalScrollBar ()->setValue (RESTORE_SETTING_SESSION (TableViewHorizontalScrollBar, 0, Int));
	ui->tableView->verticalScrollBar ()->setValue (RESTORE_SETTING_SESSION (TableViewVerticalScrollBar, 0, Int));

	ui->listView->horizontalScrollBar ()->setValue (RESTORE_SETTING_SESSION (ListViewHorizontalScrollBar, 0, Int));
	ui->listView->verticalScrollBar ()->setValue (RESTORE_SETTING_SESSION (ListViewVerticalScrollBar, 0, Int));

	ui->textBrowserItemView->horizontalScrollBar ()->setValue (RESTORE_SETTING_SESSION (TextBrowseItemViewHorizontalScrollBar, 0, Int));
	ui->textBrowserItemView->verticalScrollBar ()->setValue (RESTORE_SETTING_SESSION (TextBrowseItemViewVerticalScrollBar, 0, Int));

	ui->textBrowserItemView->horizontalScrollBar ()->setValue (RESTORE_SETTING_SESSION (TableViewForEventsHorizontalScrollBar, 0, Int));
	ui->textBrowserItemView->verticalScrollBar ()->setValue (RESTORE_SETTING_SESSION (TableViewForEventsVerticalScrollBar, 0, Int));
}

QString SettingsManager::sessionFilePath ()
{
	return QDir::homePath() + "/." + s_FamilierApplicationName + "_" + qApp->sessionId ()
			+ "_" + qApp->sessionKey ();
}

bool SettingsManager::currentViewIsTreeView ()
{
	return s_LastCurrentView == e_FamilyTreeView || s_LastCurrentView == e_TreeView;
}

bool SettingsManager::currentViewIsTableView ()
{
	return s_LastCurrentView == e_TableView || currentViewIsListView ();
}

bool SettingsManager::currentViewIsListView ()
{
	return s_LastCurrentView == e_ListView	|| s_LastCurrentView == e_IconView;
}

QAbstractItemView* SettingsManager::currentView ()
{
	switch (s_LastCurrentView)
	{
	case e_FamilyTreeView:
		return ui->familyTreeView;
	case e_TreeView:
		return ui->treeView;
	case e_TableView:
		return ui->tableView;
	case SettingsManager::e_ListView:
	case SettingsManager::e_IconView:
		return ui->listView;
	default:
		break;
	}

	return NULL;
}

QAbstractItemModel* SettingsManager::currentModel ()
{
	QAbstractItemView *view = currentView ();
	if (view)
		return view->model ();

	return NULL;
}

QModelIndex SettingsManager::currentIndexForCurrentView ()
{
	QAbstractItemView *view = currentView ();
	if (view)
		return view->currentIndex ();

	return QModelIndex ();
}

QModelIndex SettingsManager::currentIndexForCurrentViewForTreeModel ()
{
	QModelIndex currentIndex = currentIndexForCurrentView ();
	if (!currentIndex.isValid ())
		return QModelIndex ();

	QAbstractItemModel *currentModel = const_cast <QAbstractItemModel*>(currentIndex.model ());

	switch (SettingsManager::s_LastCurrentView)
	{
	case SettingsManager::e_FamilyTreeView:
	case SettingsManager::e_TreeView:
		return currentIndex;
	case SettingsManager::e_TableView:
		return dynamic_cast<TableModel*>(currentModel)->mapToSource (currentIndex);
	case SettingsManager::e_ListView:
	case SettingsManager::e_IconView:
		return dynamic_cast<IconModel*>(currentModel)->mapToSource (currentIndex);
	default:
		break;
	}

	return QModelIndex ();
}

TreeItem* SettingsManager::currentItemForCurrentView ()
{
	QAbstractItemModel *model = currentModel ();
	QModelIndex index = currentIndexForCurrentView ();

	if (!model || !index.isValid ())
		return NULL;

	if (currentViewIsTreeView ())
	{
		TreeModel *treemodel = dynamic_cast <TreeModel*>(model);
		return treemodel->getItem (index);
	}

	if (currentViewIsTableView ())
	{
		TableModel *tableModel = dynamic_cast <TableModel*>(model);
		return tableModel->getItem (index);
	}

	return NULL;
}

TreeItem* SettingsManager::itemForCurrentView (const QModelIndex &index)
{
	QAbstractItemModel *model = const_cast <QAbstractItemModel*>(index.model ());
	if (currentViewIsTreeView ())
		return dynamic_cast<TreeModel*>(model)->getItem (index);
	if (currentViewIsTableView ())
		return dynamic_cast<TableModel*>(model)->getItem (index);

	return NULL;
}
