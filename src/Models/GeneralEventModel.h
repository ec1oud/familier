#ifndef GENERALEVENTMODEL_H
#define GENERALEVENTMODEL_H

#include <QSortFilterProxyModel>

class GeneralEventModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit GeneralEventModel(QObject *parent = 0);

	virtual QModelIndex 	mapFromSource		(const QModelIndex& sourceIndex) const;
	virtual QModelIndex	mapToSource		(const QModelIndex& proxyIndex) const;
	virtual QVariant	headerData		(int section, Qt::Orientation orientation, int role) const;
	virtual int		columnCount		(const QModelIndex& parent) const;

signals:
	void			memberListChanged	() const;

public slots:
	void			handleMemberListChange	() const;

};

#endif // GENERALEVENTMODEL_H
