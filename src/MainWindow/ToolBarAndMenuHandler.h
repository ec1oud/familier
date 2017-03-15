#ifndef TOOLBARANDMENUHANDLER_H
#define TOOLBARANDMENUHANDLER_H

#include <QObject>
#include "ui_MainWindow.h"
#include <QMenu>
#include "TreeModel.h"
#include "IconModel.h"

class ToolBarAndMenuHandler : public QObject
{
	Q_OBJECT

	Ui::MainWindow *	ui;

	TreeModel *		_treeModel;
	IconModel *		_iconModel;

public:
	explicit ToolBarAndMenuHandler(Ui::MainWindow *_ui, QObject *parent = 0);

signals:
	void		needToScrollTo				(QModelIndex);

public slots:
	void		disableAllToolItemsForEditing		();
	void		updateToolBarAndMenuItemsForIndexChange	(QModelIndex currentIndex);
	void		customContextMenuRequestHandler		(QPoint point);
	void		handleAlterEgoClick			(ulong id);
};

#endif // TOOLBARANDMENUHANDLER_H
