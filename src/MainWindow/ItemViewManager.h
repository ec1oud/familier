#ifndef ITEMVIEWMANAGER_H
#define ITEMVIEWMANAGER_H

#include <QObject>
#include "ui_MainWindow.h"
#include "TreeModel.h"

class ItemViewManager : public QObject
{
	Q_OBJECT

	Ui::MainWindow *	ui;
	TreeModel *		_treeModel;

public:
	explicit ItemViewManager(Ui::MainWindow *_ui, QObject *parent = 0);

	QModelIndex	getTreeModelIndexForSearchItem	(TreeItem *item, const bool &isIndividual,
							 const IDTYPE &id,
							 const IDTYPE &spouseIdForCurrentEntryOfFemale
							 ) const;
	void		handleAnchorClick		(const QString &url) const;

signals:
	void		lastActiveIndexesRequireUpdation(QModelIndex index) const;
	void		modelViewsRequireScrolling	(QModelIndex index) const;
	void		entryNeedsToBeEdited		(QModelIndex index) const;

public slots:
	void		setCurrentItem			(const QModelIndex &index) const;
	void		handleAnchorClick		(const QUrl &url) const;
	void		updateItem			() const;
};

#endif // ITEMVIEWMANAGER_H
