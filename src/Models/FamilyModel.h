#ifndef FAMILYMODEL_H
#define FAMILYMODEL_H

#include <QSortFilterProxyModel>

class FamilyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit FamilyModel(QObject *parent = 0);

	virtual QModelIndex 	mapFromSource		(const QModelIndex& sourceIndex) const;
	virtual QModelIndex	mapToSource		(const QModelIndex& proxyIndex) const;
	virtual int		rowCount		(const QModelIndex& parent) const;

signals:

public slots:

};

#endif // FAMILYMODEL_H
