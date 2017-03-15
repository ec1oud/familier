#ifndef NAVIGATEMENUHANDLER_H
#define NAVIGATEMENUHANDLER_H

#include <QObject>
#include <QVector>
#include "ui_MainWindow.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "TableModel.h"
#include "IconModel.h"
#include "FamilyTreeModel.h"

class NavigateMenuHandler : public QObject
{
	Q_OBJECT

	Ui::MainWindow *			ui;

	QVector <QModelIndex>			lastActiveIndexsVector;
	int					currentActiveIndex;

	TreeModel *				_treeModel;
	TableModel *				_tableModel;
	IconModel *				_iconModel;
	FamilyTreeModel *			_familyTreeModel;

public:
	explicit NavigateMenuHandler(Ui::MainWindow *_ui, QObject *parent = 0);

	void		updateGotoActionItems			();
	QStringList	getOpenedItemsAsUrls			();

signals:
	void		itemViewNeedsUpdation			(QModelIndex index);

public slots:
	void		scrollTo				(QModelIndex index);
	void		gotoPrevious				();
	void		gotoNext				();
	void		appendIntoLastActiveIndexes		(QModelIndex index);
	void		clearLastActiveIndexes			();
};

#endif // NAVIGATEMENUHANDLER_H
