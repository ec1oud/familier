#ifndef RICHTEXTEDITORDELEGATE_H
#define RICHTEXTEDITORDELEGATE_H

#include <QItemDelegate>
#include "IconModel.h"
#include "FamilyModel.h"

class RichTextEditorDelegate : public QItemDelegate
{
	Q_OBJECT

	IconModel *		_iconModel;
	FamilyModel *		_familyModel;

public:
	explicit RichTextEditorDelegate(IconModel *iconModel, FamilyModel *familyModel,
					QObject *parent = 0);

	QWidget*	createEditor		(QWidget* parent, const QStyleOptionViewItem &option,
						 const QModelIndex& index ) const;
	void		setEditorData		(QWidget* editor, const QModelIndex & index ) const;
	void		setModelData		(QWidget *editor, QAbstractItemModel *model,
						 const QModelIndex &index) const;
	void		updateEditorGeometry	(QWidget *editor, const QStyleOptionViewItem &option,
						 const QModelIndex &/* index */) const;
signals:

public slots:

};

#endif // RICHTEXTEDITORDELEGATE_H
