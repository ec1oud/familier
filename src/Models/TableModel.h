#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QSortFilterProxyModel>
#include "TreeItem.h"

class TableModel : public QSortFilterProxyModel
{
	Q_OBJECT

protected:
	int				_sortColumn;
	Qt::SortOrder			_sortOrder;
	QList <TreeItem *>		_individualItemsSortedList;

public:
	TableModel (QObject * parent = 0);

	virtual QModelIndex 	mapFromSource	(const QModelIndex& sourceIndex) const;
	virtual QModelIndex	mapToSource	(const QModelIndex& proxyIndex) const;
	virtual QModelIndex	index		(int, int, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex 	parent		(const QModelIndex& index) const;
	virtual int		rowCount	(const QModelIndex& parent) const;
	virtual int		columnCount	(const QModelIndex& parent) const;
	virtual QVariant	data		(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QVariant	headerData	(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual void		sort		(int column, Qt::SortOrder order = Qt::AscendingOrder);

	void			clear		();

	virtual TreeItem*	getItem		(const QModelIndex &index) const;

protected:
	void			populateModel	();

public slots:
	virtual void		regenerateModel	();
	void			handleItemDumminessChange (TreeItem *item);

public:
	void			setupSignals	();
	void			breakSignals	();
};

#endif /* TABLEMODEL_H */
