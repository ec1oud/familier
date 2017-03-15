#ifndef ICONMODEL_H
#define ICONMODEL_H

#include "TableModel.h"

class IconModel : public TableModel
{
	Q_OBJECT

public:
	explicit IconModel(QObject *parent = 0);

	virtual QVariant	data					(const QModelIndex& index, int role = Qt::DisplayRole) const;

	bool			shouldShowItemsWOImagesInIconView	() const;

private:
	void			adjustItemVisibilityInIconView		();

public slots:
	bool			setShouldShowItemsWOImagesInIconView	(bool shouldShowItemsWOImagesInIconView);
	virtual void		regenerateModel				();

signals:

public slots:

};

#endif // ICONMODEL_H
