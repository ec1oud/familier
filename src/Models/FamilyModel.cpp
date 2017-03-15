#include "FamilyModel.h"

FamilyModel::FamilyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

QModelIndex FamilyModel::mapFromSource (const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid () || sourceIndex.parent ().isValid ())
		return QModelIndex();

	return index (sourceIndex.row (), sourceIndex.column (), sourceIndex.parent ());
}

QModelIndex FamilyModel::mapToSource (const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid ())
		return QModelIndex();

	return sourceModel ()->index (proxyIndex.row (), proxyIndex.column (), proxyIndex.parent ());
}

int FamilyModel::rowCount (const QModelIndex &parent) const
{
	if (parent.isValid ())
		return 0;

	return sourceModel ()->rowCount (QModelIndex());
}
