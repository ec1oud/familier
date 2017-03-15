#include "IconModel.h"
#include "TreeModel.h"
#include "TreeItem.h"
#include <QIcon>
#include "SettingsManager.h"

IconModel::IconModel(QObject *parent) : TableModel(parent)
{
}

QVariant IconModel::data(const QModelIndex &index, int role) const
{
	int row = index.row ();
	int column = index.column();
	if (index.isValid () && row >=0 && row < _individualItemsSortedList.size () &&
			column == Familier::e_col_name && role == Qt::DecorationRole)
	{
		TreeItem *item = _individualItemsSortedList.at (index.row ());
		Individual *individual = dynamic_cast<Individual*>(item->fam());
		if (individual)
			return individual->getIcon ()->pixmap (60, 60);
	}
	return TableModel::data(index, role);
}

bool IconModel::shouldShowItemsWOImagesInIconView () const
{
	return SettingsManager::s_IconViewShowIndividualsWOImages;
}

void IconModel::adjustItemVisibilityInIconView ()
{
	int countOfItemsWithImage = 0;
	while (_individualItemsSortedList.size () - countOfItemsWithImage)
	{
		TreeItem *item = _individualItemsSortedList.at (countOfItemsWithImage);
		if (!item->hasImage ())
			_individualItemsSortedList.removeOne (item);
		else
			countOfItemsWithImage ++;
	}
}

bool IconModel::setShouldShowItemsWOImagesInIconView (bool shouldShowItemsWOImagesInIconView)
{
	SettingsManager::s_IconViewShowIndividualsWOImages = shouldShowItemsWOImagesInIconView;

	if (shouldShowItemsWOImagesInIconView)
		regenerateModel ();
	else
	{
		beginResetModel ();
		adjustItemVisibilityInIconView ();
		endResetModel ();
	}
	return true;
}

void IconModel::regenerateModel ()
{
	beginResetModel ();
	populateModel ();
	if (!shouldShowItemsWOImagesInIconView ())
		adjustItemVisibilityInIconView ();
	endResetModel ();
}
