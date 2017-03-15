#ifndef FILEMENUHANDLER_H
#define FILEMENUHANDLER_H

#include <QObject>
#include <QAction>
#include "ui_MainWindow.h"
#include "TreeModel.h"
#include "TableModel.h"
#include "IconModel.h"
#include "EventModel.h"
#include <QProgressDialog>

class FileMenuHandler : public QObject
{
	Q_OBJECT

	Ui::MainWindow *			ui;

	bool					_documentModified;
	QList<QAction *>			_recentFileActionsList;

	TreeModel *				_treeModel;
	TableModel *				_tableModel;
	IconModel *				_iconModel;
	EventModel *				_eventModel;

public:
	explicit FileMenuHandler(EventModel* eventModel, Ui::MainWindow *_ui, QObject *parent = 0);

	bool		getDocumentModified		() const { return _documentModified; }
	bool		openFile			(QString fileName);
	bool		saveIfRequired			();

private:
	void		initializeRecentFileActions	();
	bool		closeFileWithoutSave		();
	bool		maybeSave			();
	void		initialiseModel			(QProgressDialog &progress);
	void		clearModel			();
	void		unsetDocumentModified		();
	void		updateRecentFileActions		();

signals:
	void		windowTitleRequiresChange	(QString);
	void		statusBarNeedsUpdation		();
	void		navigationItemsNeedClearing	();
	void		itemViewNeedsClearing		();
	void		exitFromApplication		();
	void		modelLoadingFinished		();

public slots:
	bool		newFile				();
	bool		openFile			();
	bool		openRecentFile			(const unsigned long recentFileNo);
	bool		closeFile			();
	bool		saveFile			();
	bool		saveFileAs			();
	bool		exit				();
	bool		importFile			();
	bool		exportFile			();
	void		setDocumentModified		();
};

#endif // FILEMENUHANDLER_H
