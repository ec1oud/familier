#ifndef EVENTLINEMODEL_H
#define EVENTLINEMODEL_H

#include <QSortFilterProxyModel>
#include "EventModel.h"

class EventLineModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	enum t_columns
	{
		e_col_Year = EventModel::colMaximum,
		e_col_Month,
		e_col_Day,
		e_col_Maximum
	};

public:
	explicit EventLineModel(QObject *parent = 0);

	virtual QModelIndex 	mapFromSource		(const QModelIndex& sourceIndex) const;
	virtual QModelIndex	mapToSource		(const QModelIndex& proxyIndex) const;
	virtual QModelIndex	index			(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex	parent			(const QModelIndex &child) const;
	virtual QVariant	headerData		(int section, Qt::Orientation orientation, int role) const;
	virtual int		columnCount		(const QModelIndex& parent) const;
	virtual QVariant	data			(const QModelIndex &index, int role) const;

signals:

public slots:

};

#endif // EVENTLINEMODEL_H
