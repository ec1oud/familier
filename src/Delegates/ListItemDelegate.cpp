#include "ListItemDelegate.h"
#include <QListView>

ListItemDelegate::ListItemDelegate(QObject *parent) :
	QAbstractItemDelegate(parent)
{
}

void ListItemDelegate::setEditorData (QWidget *editor, const QModelIndex &index) const
{
	QListView* tabView = qobject_cast<QListView*>(editor);

	const QAbstractItemModel *model = index.model();
	if (!tabView || !model)
		QStyledItemDelegate::setEditorData(editor, index);
}
