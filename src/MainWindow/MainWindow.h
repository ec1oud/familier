#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "FileMenuHandler.h"
#include "EditMenuHandler.h"
#include "NavigateMenuHandler.h"
#include "ViewMenuHandler.h"
#include "HelpMenuHandler.h"
#include "ToolBarAndMenuHandler.h"
#include "StatusBarManager.h"
#include "ItemViewManager.h"
#include "EventPageManager.h"
#include "TreeModel.h"
#include "FamilyTreeModel.h"
#include "TableModel.h"
#include "IconModel.h"
#include "FamilyModel.h"
#include "EventModel.h"
#include "GeneralEventModel.h"
#include "EventLineModel.h"
#include "StatisticsPageManager.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	Ui::MainWindow			*ui;

	/* Settings must be loaded before everything else!!! */
	SettingsManager *		_settingsManager;

	TreeModel *			_treeModel;
	FamilyTreeModel *		_familyTreeModel;
	TableModel *			_tableModel;
	IconModel *			_iconModel;
	FamilyModel *			_familyModel;
	EventModel *			_eventModel;
	GeneralEventModel *		_generalEventModel;
	EventLineModel *		_eventLineModel;

	FileMenuHandler *		_fileMenuHandler;
	EditMenuHandler *		_editMenuHandler;
	NavigateMenuHandler *		_navigateMenuHandler;
	ViewMenuHandler *		_viewMenuHandler;
	HelpMenuHandler *		_helpMenuHandler;
	ToolBarAndMenuHandler *		_toolBarMenuHandler;
	StatusBarManager *		_statusBarManager;
	ItemViewManager *		_itemViewManager;
	EventPageManager *		_eventPageManager;
	StatisticsPageManager *		_statisticsPageManager;

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void		commitData				(QSessionManager &sm);
	QStringList	getOpenedItemsAsUrls			();
	void		startupBasedOnSettings			();

protected:
	virtual void	closeEvent				(QCloseEvent *closeEvent);
	virtual void	changeEvent				(QEvent *e);

private:
	bool		promptForPasswordAndValidate		();
	void		undockMainWindow			();
	void		lockMainWindow				();

public slots:
	void		clearItemView				();
	void		sort					();
	void		setCurrentItemAndUpdateIndexesList	(QModelIndex index);

	void		showMinimized				();
	void		restoreMainWindow			();
	void		dockMainWindow				();
	void		lockAndMinimizeMainWindow		();
	void		unlockMainWindow			();
};

#endif // MAINWINDOW_H
