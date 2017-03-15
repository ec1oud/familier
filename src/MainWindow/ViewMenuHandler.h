#ifndef VIEWMENUHANDLER_H
#define VIEWMENUHANDLER_H

#include <QObject>
#include "TreeModel.h"
#include "ui_MainWindow.h"

class ViewMenuHandler : public QObject
{
	Q_OBJECT

	Ui::MainWindow *			ui;
	TreeModel *				_treeModel;

public:
	explicit ViewMenuHandler(Ui::MainWindow *_ui, QObject *parent = 0);

	void		setLocked				(bool locked);

private:
	void		setCurrentEntryExpanded			(bool expand);
	void		setAllSiblingEntriesExpanded		(bool expand);
	void		resizeToContents			();

signals:
	//Model regeneration should never happen, this should be removed in future!!
	void		modelRequiresRegeneration		();
	void		lockAndMinimizeMainWindow		();
	void		unlockMainWindow			();

public slots:
	void		expandCurrentEntry			();
	void		collapseCurrentEntry			();

	void		expandAllSiblingEntries			();
	void		collapseAllSiblingEntries		();

	void		expandAllEntries			();
	void		collapseAllEntries			();

	void		lockOrUnlockMainWindow			();

	void		switchViewToTreeView			();
	void		switchViewToTableView			();
	void		switchViewToListView			();
	void		switchViewToIconView			();
	void		switchViewToFamilyTreeViewTab		();

	void		handleTabSwitch				(int);

	void		regenerateTree				();
};

#endif // VIEWMENUHANDLER_H
