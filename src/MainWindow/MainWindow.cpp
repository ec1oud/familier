#include "MainWindow.h"
#include "SettingsManager.h"
#include <QCloseEvent>
#include "TableViewDelegate.h"
#include "RichTextEditorDelegate.h"
#include <QSessionManager>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow (parent), ui (new Ui::MainWindow),
	_treeModel (new TreeModel),
	_familyTreeModel (new FamilyTreeModel),
	_tableModel (new TableModel),
	_iconModel (new IconModel),
	_familyModel (new FamilyModel),
	_eventModel (new EventModel),
	_generalEventModel (new GeneralEventModel),
	_eventLineModel (new EventLineModel)
{
	ui->setupUi(this);
	ui->dockWidgetSearchResults->setHidden (true);
	SettingsManager::initialize (ui);
	_settingsManager = new SettingsManager (this);
	_familyTreeModel->setSourceModel (_treeModel);
	_tableModel->setSourceModel(_treeModel);
	_iconModel->setSourceModel(_treeModel);
	_familyModel->setSourceModel (_treeModel);
	_generalEventModel->setSourceModel (_eventModel);
	_eventLineModel->setSourceModel (_eventModel);

	ui->treeView->setDropIndicatorShown(true);

	QItemSelectionModel *m = ui->treeView->selectionModel();
	ui->treeView->setModel(_treeModel);
	if (m) delete m;

	m = ui->tableView->selectionModel();
	ui->tableView->setModel(_tableModel);
	ui->tableView->setSortingEnabled (true);
	if (m) delete m;

	m = ui->listView->selectionModel();
	ui->listView->setModel(_iconModel);
	if (m) delete m;

	m = ui->tableViewForEvents->selectionModel ();
	ui->tableViewForEvents->setModel (_generalEventModel);
	if (m) delete m;

	ui->tableViewForEvents->setItemDelegateForColumn (EventModel::colMembers - 1,
							  new TableViewDelegate (_iconModel));
	ui->tableViewForEvents->setItemDelegateForColumn (EventModel::colDescription - 1,
							  new RichTextEditorDelegate(_iconModel, _familyModel));

	ui->familyTreeView->setModel (_familyTreeModel);
	ui->familyTreeView->setContextMenuPolicy (Qt::CustomContextMenu);

	ui->eventTimeLineView->setModel (_eventLineModel);

	_fileMenuHandler = new FileMenuHandler (_eventModel, ui, this);
	_editMenuHandler = new EditMenuHandler (_familyModel, _eventModel, ui, this);
	_navigateMenuHandler = new NavigateMenuHandler (ui, this);
	_viewMenuHandler = new ViewMenuHandler (ui, this);
	_helpMenuHandler = new HelpMenuHandler (ui, this);
	_toolBarMenuHandler = new ToolBarAndMenuHandler (ui, this);
	_statusBarManager = new StatusBarManager (ui, this);
	_itemViewManager = new ItemViewManager (ui, this);
	_eventPageManager = new EventPageManager (_eventModel, ui, this);
	_statisticsPageManager = new StatisticsPageManager (ui, this);

	connect (_fileMenuHandler, SIGNAL(windowTitleRequiresChange(QString)),
		 this, SLOT(setWindowTitle(QString)));
	connect (_fileMenuHandler, SIGNAL(statusBarNeedsUpdation()),
		 _statusBarManager, SLOT(updateItemCountInStatusBar()));
	connect (_fileMenuHandler, SIGNAL(statusBarNeedsUpdation()),
		 _statusBarManager, SLOT(updateTodayEventInStatusBar()));
	connect (_fileMenuHandler, SIGNAL(navigationItemsNeedClearing()),
		 _navigateMenuHandler, SLOT(clearLastActiveIndexes()));
	connect (_fileMenuHandler, SIGNAL(navigationItemsNeedClearing()),
		 _toolBarMenuHandler, SLOT(disableAllToolItemsForEditing()));
	connect (_fileMenuHandler, SIGNAL(itemViewNeedsClearing()),
		 this, SLOT(clearItemView()));
	connect (_fileMenuHandler, SIGNAL(exitFromApplication()), qApp, SLOT(quit()));
	connect (_fileMenuHandler, SIGNAL(modelLoadingFinished()),
		 _viewMenuHandler, SLOT(expandAllEntries()));
	connect (_fileMenuHandler, SIGNAL(modelLoadingFinished()), this, SLOT(sort()));
	connect (_fileMenuHandler, SIGNAL(modelLoadingFinished()),
		 _statisticsPageManager, SLOT(regenerateAllPieViews()));

	//connect (_editMenuHandler, SIGNAL(currentEntryRequiresExpansion()),
	//	 _viewMenuHandler, SLOT(expandCurrentEntry()));
	connect (_editMenuHandler, SIGNAL(documentModified()),
		 _fileMenuHandler, SLOT(setDocumentModified()));
	/*connect (_editMenuHandler, SIGNAL(modelRequiresRegeneration()),
		 _fileMenuHandler, SLOT(regenerateModel()));*/
	connect (_editMenuHandler, SIGNAL(itemViewRequiresUpdate()),
		 _itemViewManager, SLOT(updateItem()));
	connect (_editMenuHandler, SIGNAL(systemTrayNeedsToBeShown(bool)),
		 _statusBarManager, SLOT(setSystemTrayHidden(bool)));

	connect (_navigateMenuHandler, SIGNAL(itemViewNeedsUpdation(QModelIndex)),
		 _itemViewManager, SLOT(setCurrentItem(QModelIndex)));

	/*connect (_viewMenuHandler, SIGNAL(modelRequiresRegeneration()),
		 _fileMenuHandler, SLOT(regenerateModel()));*/
	connect (_viewMenuHandler, SIGNAL(lockAndMinimizeMainWindow()),
		 this, SLOT(lockAndMinimizeMainWindow()));
	connect (_viewMenuHandler, SIGNAL(unlockMainWindow()),
		 this, SLOT(unlockMainWindow()));

	connect (_toolBarMenuHandler, SIGNAL(needToScrollTo(QModelIndex)),
		 _navigateMenuHandler, SLOT(scrollTo(QModelIndex)));

	connect (_statusBarManager->getTodaysEventLabel (), SIGNAL(linkActivated(QUrl)),
		 _itemViewManager, SLOT(handleAnchorClick(QUrl)));
	connect (_statusBarManager, SIGNAL(showMainWindow()), this, SLOT(restoreMainWindow()));
	connect (_statusBarManager, SIGNAL(hideMainWindow()), this, SLOT(showMinimized()));

	connect (_itemViewManager, SIGNAL(lastActiveIndexesRequireUpdation(QModelIndex)),
		 _navigateMenuHandler, SLOT(appendIntoLastActiveIndexes(QModelIndex)));
	connect (_itemViewManager, SIGNAL(modelViewsRequireScrolling(QModelIndex)),
		 _navigateMenuHandler, SLOT(scrollTo(QModelIndex)));
	connect (_itemViewManager, SIGNAL(entryNeedsToBeEdited(QModelIndex)),
		 _editMenuHandler, SLOT(editEntry(QModelIndex)));

	connect (_eventPageManager, SIGNAL(documentModified()),
		 _fileMenuHandler, SLOT(setDocumentModified()));

	connect (ui->treeView, SIGNAL(clicked(QModelIndex)),
		 this, SLOT(setCurrentItemAndUpdateIndexesList(QModelIndex)));
	connect (ui->treeView, SIGNAL(activated(QModelIndex)),
		 _toolBarMenuHandler, SLOT(updateToolBarAndMenuItemsForIndexChange(QModelIndex)));
	connect (ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
		 _toolBarMenuHandler, SLOT(customContextMenuRequestHandler(QPoint)));

	connect (ui->tableView, SIGNAL(clicked(QModelIndex)),
		 this, SLOT(setCurrentItemAndUpdateIndexesList(QModelIndex)));
	connect (ui->tableView, SIGNAL(activated(QModelIndex)),
		 _toolBarMenuHandler, SLOT(updateToolBarAndMenuItemsForIndexChange(QModelIndex)));
	connect (ui->tableView, SIGNAL(customContextMenuRequested(QPoint)),
		 _toolBarMenuHandler, SLOT(customContextMenuRequestHandler(QPoint)));

	connect (ui->listView, SIGNAL(clicked(QModelIndex)),
		 this, SLOT(setCurrentItemAndUpdateIndexesList(QModelIndex)));
	connect (ui->listView, SIGNAL(activated(QModelIndex)),
		 _toolBarMenuHandler, SLOT(updateToolBarAndMenuItemsForIndexChange(QModelIndex)));
	connect (ui->listView, SIGNAL(customContextMenuRequested(QPoint)),
		 _toolBarMenuHandler, SLOT(customContextMenuRequestHandler(QPoint)));

	connect (ui->textBrowserItemView, SIGNAL(anchorClicked(QUrl)),
		 _itemViewManager, SLOT(handleAnchorClick(QUrl)));
	connect (ui->textBrowserSearchResults, SIGNAL(anchorClicked(QUrl)),
		 _itemViewManager, SLOT(handleAnchorClick(QUrl)));

	connect (ui->familyTreeView, SIGNAL(clicked(QModelIndex)),
		 this, SLOT(setCurrentItemAndUpdateIndexesList(QModelIndex)));
	connect (ui->familyTreeView, SIGNAL(activated(QModelIndex)),
		 _toolBarMenuHandler, SLOT(updateToolBarAndMenuItemsForIndexChange(QModelIndex)));
	connect (ui->familyTreeView, SIGNAL(customContextMenuRequested(QPoint)),
		 _toolBarMenuHandler, SLOT(customContextMenuRequestHandler(QPoint)));

	connect (_treeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		 _itemViewManager, SLOT(setCurrentItem(QModelIndex)));
	connect (_treeModel, SIGNAL(modelReset()), _fileMenuHandler, SLOT(setDocumentModified()));
	connect (_treeModel, SIGNAL(rowInserted()), _fileMenuHandler, SLOT(setDocumentModified()));
	connect (_treeModel, SIGNAL(expandAssociatedTree()),
		 _viewMenuHandler, SLOT(expandAllEntries()));
	connect (_treeModel, SIGNAL(itemDumminessChanged(TreeItem*)),
		 _tableModel, SLOT(handleItemDumminessChange(TreeItem*)));
	connect (_treeModel, SIGNAL(itemDumminessChanged(TreeItem*)),
		 _statusBarManager, SLOT(updateItemCountInStatusBar()));
	connect (_treeModel, SIGNAL(newEventAdded()), _eventModel, SLOT(regenerateModel()));

	connect (_eventModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		 _fileMenuHandler, SLOT(setDocumentModified()));
	connect (_eventModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		 ui->familyTreeView, SLOT(repaint()));
	connect (_eventModel, SIGNAL(eventModelRegenerated()),
		 ui->tableViewForEvents, SLOT(resizeColumnsToContents()));

	connect (_generalEventModel, SIGNAL(memberListChanged()),
		 _fileMenuHandler, SLOT(setDocumentModified()));

	connect (this, SIGNAL(destroyed()), _fileMenuHandler, SLOT(exit()));
	connect (this, SIGNAL(destroyed(QObject*)), _fileMenuHandler, SLOT(exit()));
	connect (this, SIGNAL(customContextMenuRequested(QPoint)),
		 _toolBarMenuHandler, SLOT(customContextMenuRequestHandler(QPoint)));

	qApp->setApplicationName (SettingsManager::s_FamilierApplicationName);
	qApp->setApplicationVersion (SettingsManager::s_FamilierVersion);
	qApp->setOrganizationDomain (SettingsManager::s_FamilierDomain);
	qApp->setOrganizationName (SettingsManager::s_FamilierOrganisation);
	setWindowTitle(SettingsManager::s_FamilierApplicationTitle);
	qApp->setQuitOnLastWindowClosed (false);

	showMaximized();
}

MainWindow::~MainWindow()
{
	delete ui;

	delete _eventLineModel;
	delete _generalEventModel;
	delete _eventModel;
	delete _familyModel;
	delete _iconModel;
	delete _tableModel;
	delete _familyTreeModel;
	delete _treeModel;
}

void MainWindow::commitData (QSessionManager &sm)
{
	if (!_fileMenuHandler->saveIfRequired ())
		sm.cancel ();
	sm.release ();
}

QStringList MainWindow::getOpenedItemsAsUrls ()
{
	return _navigateMenuHandler->getOpenedItemsAsUrls ();
}

void MainWindow::startupBasedOnSettings ()
{
	if (SettingsManager::s_StartupLocked)
		lockMainWindow ();

	if (SettingsManager::s_StartupMinimized || SettingsManager::s_StartupLocked)
		showMinimized ();

	if (!SettingsManager::s_DoNotRestoreLastSession && qApp->isSessionRestored ())
		SettingsManager::restoreState (_fileMenuHandler, _itemViewManager);
	else if (SettingsManager::s_LoadLastDocumentAutomatically)
		_fileMenuHandler->openRecentFile (0);
}

void MainWindow::closeEvent(QCloseEvent *closeEvent)
{
	if (SettingsManager::s_EnableSystemTray)
	{
		showMinimized ();
		closeEvent->ignore ();
		return;
	}

	if (!_fileMenuHandler->exit())
		closeEvent->ignore();
}

void MainWindow::changeEvent (QEvent *e)
{
	if (e->type () == QEvent::WindowStateChange)
	{
		QWindowStateChangeEvent *wsce = (QWindowStateChangeEvent *) e;
		if (wsce->oldState () != Qt::WindowMinimized && isMinimized ())
		{
			switch (wsce->oldState ())
			{
			case Qt::WindowNoState:
				showNormal ();
				break;
			case Qt::WindowMaximized:
				showMaximized ();
				break;
			case Qt::WindowFullScreen:
				showFullScreen ();
				break;
			default:
				break;
			}

			showMinimized ();
			e->ignore ();
		}
	}

	QMainWindow::changeEvent (e);
}

bool MainWindow::promptForPasswordAndValidate ()
{
	QString password;
	bool ok;

	for (int i = 0; i < 3; i++)
	{
		password = QInputDialog::getText (this, "Enter Password", "Password",
						  QLineEdit::Password, "", &ok);

		if (!ok)
		{
			QMessageBox::information (this, "Unlocking canceled",
						  "Unlocking has been canceled by the user.");
			return false;
		}

		if (SettingsManager::s_Password == password)
			return true;
	}

	QMessageBox::warning (this, "Invalid Passwords", "Wrong password was entered three times. "
			      "As a result, unlocking has been canceled.");
	return false;
}

void MainWindow::undockMainWindow ()
{
	if (SettingsManager::s_MainWindowIsLocked)
		return;

	setVisible (true);
	_statusBarManager->setDocked (false);
}

void MainWindow::lockMainWindow ()
{
	SettingsManager::s_MainWindowIsLocked = true;
	_viewMenuHandler->setLocked (true);
}

void MainWindow::clearItemView()
{
	_itemViewManager->setCurrentItem(QModelIndex());
}

void MainWindow::sort ()
{
	ui->tableView->sortByColumn (0, Qt::AscendingOrder);
}

void MainWindow::setCurrentItemAndUpdateIndexesList(QModelIndex index)
{
	_toolBarMenuHandler->updateToolBarAndMenuItemsForIndexChange(index);
	if (SettingsManager::s_LastCurrentView != SettingsManager::e_FamilyTreeView)
		_itemViewManager->setCurrentItem(index);
	_navigateMenuHandler->appendIntoLastActiveIndexes(index);
}

void MainWindow::showMinimized ()
{
	if (SettingsManager::s_MinimizeLocksAutomatically && !SettingsManager::s_MainWindowIsLocked)
		lockMainWindow ();

	if (SettingsManager::s_EnableSystemTray && SettingsManager::s_MinimizeMinimizesToSystemTray
			&& isVisible ())
		dockMainWindow ();

	if (!SettingsManager::s_MinimizeMinimizesToSystemTray)
		QMainWindow::showMinimized ();
}

void MainWindow::restoreMainWindow ()
{
	if (SettingsManager::s_MainWindowIsLocked)
		unlockMainWindow ();

	undockMainWindow ();
}

void MainWindow::dockMainWindow ()
{
	setVisible (false);
	_statusBarManager->setDocked (true);
}

void MainWindow::lockAndMinimizeMainWindow ()
{
	lockMainWindow ();
	showMinimized ();
}

void MainWindow::unlockMainWindow ()
{
	if (!SettingsManager::s_MainWindowIsLocked ||
			(SettingsManager::s_EnablePassword && !promptForPasswordAndValidate ()))
		return;

	SettingsManager::s_MainWindowIsLocked = false;
	_viewMenuHandler->setLocked (false);
}
