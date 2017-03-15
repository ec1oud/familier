#include "RichTextEditorDelegate.h"
#include "RichTextEditor.h"
#include "GeneralEventModel.h"
#include "Event.h"

RichTextEditorDelegate::RichTextEditorDelegate (IconModel *iconModel, FamilyModel *familyModel,
						QObject *parent) : QItemDelegate(parent),
						_iconModel (iconModel), _familyModel (familyModel)
{
}

QWidget *RichTextEditorDelegate::createEditor (QWidget *parent, const QStyleOptionViewItem &option,
					       const QModelIndex &index) const
{
	if (!index.isValid ())
		return QItemDelegate::createEditor(parent, option, index);

	RichTextEditor *richEdit = new RichTextEditor(_iconModel, _familyModel);
	return richEdit;
}

void RichTextEditorDelegate::setEditorData (QWidget *editor, const QModelIndex &index) const
{
	if (!index.isValid ())
		QItemDelegate::setEditorData (editor, index);

	RichTextEditor *richEdit = qobject_cast<RichTextEditor*>(editor);
	richEdit->setText (index.data (Qt::EditRole).toString ());
}

void RichTextEditorDelegate::setModelData (QWidget *editor, QAbstractItemModel *model,
					   const QModelIndex &index) const
{
	QString changedData = dynamic_cast<RichTextEditor*>(editor)->toHtml ();
	model->setData (index, changedData, Qt::EditRole);
}

void RichTextEditorDelegate::updateEditorGeometry (QWidget *editor, const QStyleOptionViewItem &option,
						   const QModelIndex &) const
{
	int width = 662;
	int height = 480;
	QWidget *parentWidget = dynamic_cast<QWidget*>(editor->parent ());
	if (!parentWidget)
		editor->setGeometry(QRect(option.rect.x (), option.rect.y (), width, height));
	else
	{
		QRect parentWidgetRect = parentWidget->geometry ();
		QRect rect = QRect(option.rect.x (), option.rect.y (), width, height);
		if (parentWidgetRect.x () + parentWidgetRect.width () < rect.x () + rect.width ())
			rect.adjust (-width, 0, -width, 0);
		if (parentWidgetRect.y () + parentWidgetRect.height () < rect.y () + rect.width ())
			rect.adjust (0, -height, 0, -height);
		editor->setGeometry (rect);
	}
}
