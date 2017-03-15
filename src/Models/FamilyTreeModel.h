#ifndef FAMILYTREEMODEL_H
#define FAMILYTREEMODEL_H

#include <QSortFilterProxyModel>
#include <TreeItem.h>

class FamilyTreeModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit FamilyTreeModel(QObject *parent = 0);

	virtual QModelIndex 	mapFromSource	(const QModelIndex& sourceIndex) const;
	virtual QModelIndex	mapToSource	(const QModelIndex& proxyIndex) const;
	virtual QModelIndex	index		(int, int, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex		index		(TreeItem *item, int column = 0) const;
	virtual QModelIndex	parent		(const QModelIndex& index) const;
	virtual int		rowCount	(const QModelIndex& parent) const;
	virtual int		columnCount	(const QModelIndex& parent) const;
	virtual QVariant	headerData	(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags	flags		(const QModelIndex &index) const;
	virtual QVariant	data		(const QModelIndex& index, int role = Qt::DisplayRole) const;

	TreeItem *		getItem		(const QModelIndex &index) const;

signals:

public slots:

};

#endif // FAMILYTREEMODEL_H
