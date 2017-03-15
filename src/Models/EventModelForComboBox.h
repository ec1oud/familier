#ifndef EVENTMODELFORCOMBOBOX_H
#define EVENTMODELFORCOMBOBOX_H

#include <QSortFilterProxyModel>

class EventModelForComboBox : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit EventModelForComboBox(QObject *parent = 0);

	virtual QModelIndex 	mapFromSource	(const QModelIndex& sourceIndex) const;
	virtual QModelIndex	mapToSource	(const QModelIndex& proxyIndex) const;
	virtual QModelIndex	index		(int, int, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex 	parent		(const QModelIndex& index) const;
	virtual int		rowCount	(const QModelIndex& parent) const;
	virtual int		columnCount	(const QModelIndex& parent) const;
	virtual QVariant	data		(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual bool		setData		(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	virtual QVariant	headerData	(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:

public slots:

};

#endif // EVENTMODELFORCOMBOBOX_H
