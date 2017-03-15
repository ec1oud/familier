#ifndef COMPLETIONDELEGATE_H
#define COMPLETIONDELEGATE_H

#include <QItemDelegate>

class CompletionDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	explicit CompletionDelegate(QObject *parent = 0);

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

#endif // COMPLETIONDELEGATE_H
