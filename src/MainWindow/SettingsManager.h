#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QStringList>
#include <QSettings>
#include "ui_MainWindow.h"
#include <QDateTime>
#include "FileMenuHandler.h"
#include "ItemViewManager.h"

class SettingsManager : public QObject
{
	Q_OBJECT

	enum
	{
		e_LastCurrentView,
		e_LastSaveDirectory,
		e_LastExportDirectory,
		e_RecentFilesList,
		e_ProfileImageDirectory,

		e_FamilyTreeShowBasicItemDetails,
		e_FamilyTreeShowExtraItemDetails,
		e_IconViewShowIndividualsWOImages,

		e_DoNotRestoreLastSession,
		e_LoadLastDocumentAutomatically,
		e_ExportForceLastName,
		e_DisplayIndianRelations,
		e_EnableSystemTray,
		e_MinimizeMinimizesToSystemTray,
		e_StartupMinimized,
		e_StartupLocked,
		e_EnablePassword,
		e_Password,
		e_MinimizeLocksAutomatically,

		e_SessionOpenFile,
		e_SessionTabIndex,

		e_SessionFamilyTreeViewBasicDetails,
		e_SessionFamilyTreeViewExtraDetails,
		e_SessionFamilyTreeViewHorizontalScrollBar,
		e_SessionFamilyTreeViewVerticalScrollBar,

		e_SessionEventTimeLineViewHorizontalScrollBar,
		e_SessionEventTimeLineViewVerticalScrollBar,

		e_SessionStatisticsViewHorizontalScrollBar,
		e_SessionStatisticsViewVerticalScrollBar,

		e_SessionStackedWidgetIndex,

		e_SessionTreeViewHorizontalScrollBar,
		e_SessionTreeViewVerticalScrollBar,

		e_SessionTableViewHorizontalScrollBar,
		e_SessionTableViewVerticalScrollBar,

		e_SessionListViewHorizontalScrollBar,
		e_SessionListViewVerticalScrollBar,

		e_SessionTextBrowseItemViewHorizontalScrollBar,
		e_SessionTextBrowseItemViewVerticalScrollBar,
		e_SessionTextBrowserLastOpenItems,

		e_SessionTableViewForEventsHorizontalScrollBar,
		e_SessionTableViewForEventsVerticalScrollBar
	};

	static QHash <long, QString>	s_EnumToPropertyNameHash;

	static Ui::MainWindow*		ui;

public:
	static QString		s_DocumentTitle;
	static QDateTime	s_DocumentCreationDateTime;
	static QDateTime	s_DocumentModificationDateTime;
	static int		s_DocumentModificationCount;
	static QString		s_DocumentFilePath;
	static QString		s_DocumentDirectoryPath;
	static QString		s_ProfileImageDirectory;

	static QString		s_FamilierApplicationName;
	static QString		s_FamilierOrganisation;
	static QString		s_FamilierDomain;
	static QString		s_FamilierAuthor;
	static QString		s_FamilierAuthorEmail;
	static const int	s_FamilierMajorVersion;
	static const int	s_FamilierMinorVersion;
	static const int	s_FamilierPatchVersion;
	static const int	s_FamilierBuildVersion;
	static QString		s_FamilierVersion;
	static QString		s_FamilierApplicationTitle;

	static int		s_MaxRecentFiles;
	static int		s_MaxPieViewEntries;
	static int		s_MaxEditDialogImageWidth;
	static QStringList	s_RecentFilesList;
	static QString		s_LastSaveDirectory;
	static QString		s_LastExportDirectory;
	static QString		s_CWD;
	static bool		s_MainWindowIsLocked;
	static bool		s_MainWindowIsVisible;

	static bool		s_DoNotRestoreLastSession;
	static bool		s_LoadLastDocumentAutomatically;
	static bool		s_ExportForceLastName;
	static bool		s_DisplayIndianRelations;
	static bool		s_EnableSystemTray;
	static bool		s_MinimizeMinimizesToSystemTray;
	static bool		s_StartupMinimized;
	static bool		s_StartupLocked;
	static bool		s_EnablePassword;
	static QString		s_Password;
	static bool		s_MinimizeLocksAutomatically;

	static bool		s_FamilyTreeShowBasicItemDetails;
	static bool		s_FamilyTreeShowExtraItemDetails;
	static bool		s_IconViewShowIndividualsWOImages;

	static enum t_LastCurrentView
	{
		e_MinimumView,
		e_TreeView,
		e_TableView,
		e_ListView,
		e_IconView,
		e_FamilyTreeView,
		e_MaximumView
	} s_LastCurrentView;

public:
	explicit SettingsManager (QObject *parent = 0);
	~SettingsManager ();

	static void			initialize		(Ui::MainWindow *_ui);
	static void			saveState		(QSessionManager &sm,
								 const QStringList &urlsList);
	static void			saveState		(QSettings &settings,
								 const QStringList &urlsList);
	static void			restoreState		(FileMenuHandler *fmh,
								 ItemViewManager *ivm);
	static QString			sessionFilePath		();

	static bool			currentViewIsTreeView	();
	static bool			currentViewIsTableView	();
	static bool			currentViewIsListView	();
	static QAbstractItemView *	currentView		();
	static QAbstractItemModel *	currentModel		();
	static QModelIndex		currentIndexForCurrentView ();
	static QModelIndex		currentIndexForCurrentViewForTreeModel ();
	static TreeItem*		currentItemForCurrentView ();
	static TreeItem*		itemForCurrentView	(const QModelIndex &index);

signals:

public slots:

};

#endif // SETTINGSMANAGER_H
