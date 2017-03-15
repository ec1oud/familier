#include "FamilyTreeModel.h"
#include "TreeModel.h"
#include "Individual.h"

FamilyTreeModel::FamilyTreeModel(QObject *parent) :
		QSortFilterProxyModel(parent)
{
}

QModelIndex FamilyTreeModel::mapFromSource (const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid ())
		return QModelIndex();

	TreeItem *item = static_cast<TreeItem*>(sourceIndex.internalPointer ());
	if (!item)
		return QModelIndex();

	return createIndex (sourceIndex.row (), sourceIndex.column (), item);
}

QModelIndex FamilyTreeModel::mapToSource (const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid ())
		return QModelIndex();

	if (proxyIndex.column () < 0 || proxyIndex.column () >= Familier::e_col_maximum)
		return QModelIndex();

	TreeItem *item = static_cast<TreeItem*>(proxyIndex.internalPointer ());
	if (!item)
		return QModelIndex();
	return dynamic_cast<TreeModel *> (sourceModel())->index (item, proxyIndex.column ());
}

QModelIndex FamilyTreeModel::index (int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	TreeItem *parentItem = dynamic_cast<TreeModel *> (sourceModel())->getItem (parent);
	if (row < 0 || row >= parentItem->childCount ())
		return QModelIndex();

	TreeItem *childItem = parentItem->childAt (row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex FamilyTreeModel::index (TreeItem *item, int column) const
{
	if (!item || item->isRoot ())
		return QModelIndex();

	return createIndex(item->myChildNumber(), column, item);
}

QModelIndex FamilyTreeModel::parent (const QModelIndex &index) const
{
	if (index.isValid ())
	{
		TreeItem *item = static_cast<TreeItem*>(index.internalPointer ());
		if (item && item->parentItem ())
			return this->index (item->parentItem ());
	}

	return QModelIndex();
}

int FamilyTreeModel::rowCount (const QModelIndex &parent) const
{
	return sourceModel ()->rowCount (mapToSource (parent));
}

int FamilyTreeModel::columnCount (const QModelIndex &parent) const
{
	return sourceModel ()->columnCount (parent) + 1;
}

QVariant FamilyTreeModel::headerData (int section, Qt::Orientation orientation, int role) const
{
	if (section == Familier::e_col_maximum)
	{
		if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
			return "Icon";

		return QVariant();
	}

	return sourceModel ()->headerData (section, orientation, role);
}

Qt::ItemFlags FamilyTreeModel::flags (const QModelIndex &index) const
{
	if (index.column () == Familier::e_col_maximum)
		return Qt::ItemIsEnabled;

	return sourceModel ()->flags (mapToSource (index));
}

QVariant FamilyTreeModel::data (const QModelIndex &index, int role) const
{
	if (!index.isValid ())
		return QVariant();

	if (index.column () == Familier::e_col_maximum)
	{
		if (role == Qt::DecorationRole)
		{
			TreeItem *item = static_cast<TreeItem *>(index.internalPointer ());
			if (!item || !item->isIndividual ())
				return QVariant();
			Individual *individual = dynamic_cast<Individual*>(item->fam());
			if (individual)
				return *(individual->getIcon ());
		}
		else if (role == Qt::ToolTipRole)
		{
			return sourceModel ()->data (mapToSource (
					this->index (index.row (), Familier::e_col_name,
						     index.parent ())), Qt::ToolTipRole);
		}
		else
			return QVariant ();

	}

	if (role == Qt::DisplayRole)
	{
		switch (index.column ())
		{
		case Familier::e_col_name:
			{
				TreeItem *item = static_cast<TreeItem*>(index.internalPointer ());
				if (item->isDummy ())
					return "***";
			}
			break;
		case Familier::e_col_birthDate:
			return "b. " + sourceModel ()->data (mapToSource (index), role).toString ();
		case Familier::e_col_deathDate:
			return "d. " + sourceModel ()->data (mapToSource (index), role).toString ();
		case Familier::e_col_marriageDate:
			return "m. " + sourceModel ()->data (mapToSource (index), role).toString ();
		default:
			break;
		}
	}

	return sourceModel ()->data (mapToSource (index), role);
}

TreeItem* FamilyTreeModel::getItem (const QModelIndex &index) const
{
	TreeModel *treeModel = dynamic_cast<TreeModel*>(sourceModel ());
	if (!treeModel)
		return NULL;

	return treeModel->getItem (mapToSource (index));
}
