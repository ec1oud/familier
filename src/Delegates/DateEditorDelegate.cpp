#include "DateEditorDelegate.h"

#include <QVBoxLayout>
#include <QCalendarWidget>
#include <QLineEdit>

DateEditorDelegate::DateEditorDelegate(QObject *parent) :
		QStyledItemDelegate(parent)
{
}


QWidget *DateEditorDelegate::createEditor (QWidget* parent, const QStyleOptionViewItem &option,
					   const QModelIndex& index ) const
{
	Q_UNUSED (option)
	Q_UNUSED (index)

	QWidget *widget = new QWidget(parent);
	widget->setObjectName(QString::fromUtf8("widget"));
	//widget->setGeometry(QRect(120, 100, 118, 66));

	QVBoxLayout *verticalLayout = new QVBoxLayout(widget);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

	QLineEdit *ledit = new QLineEdit(widget);
	ledit->setObjectName(QString::fromUtf8("lineedit"));
	verticalLayout->addWidget(ledit);

	QCalendarWidget *cal = new QCalendarWidget(widget);
	cal->setObjectName(QString::fromUtf8("calender"));
	verticalLayout->addWidget(cal);

	//connect (cal, SIGNAL(selectionChanged()), this, SLOT())
	return widget;
}

void DateEditorDelegate::setEditorData(QWidget* editor,
				       const QModelIndex & index ) const
{
	if (!index.isValid ())
		return;

	QVBoxLayout *layout = dynamic_cast<QVBoxLayout*>(editor->layout());
	QLineEdit *ledit = dynamic_cast<QLineEdit*>(layout->itemAt(0));
	QCalendarWidget *cal = dynamic_cast<QCalendarWidget*>(layout->itemAt(1));
	QAbstractItemModel *model = const_cast<QAbstractItemModel*>(index.model());

	if (!ledit || !cal || !model)
		QStyledItemDelegate::setEditorData(editor, index);

	QVariant varDate = model->data(index, Qt::EditRole);
	QDate date = varDate.toDate();
	if (date.isValid())
	{
		ledit->setText (date.toString("dd/MM/yyyy"));
		cal->setSelectedDate(date);
	}
	else
		ledit->setText (varDate.toString ());
}

void DateEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
				      const QModelIndex &index) const
{
	if (!index.isValid())
		return;

	QVBoxLayout *layout = dynamic_cast<QVBoxLayout*>(editor->layout());
	QLineEdit *ledit = dynamic_cast<QLineEdit*>(layout->itemAt(0));

	model->setData(index, ledit->text(), Qt::DisplayRole);
	model->setData(index, ledit->text(), Qt::EditRole);
}

void DateEditorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
					      const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}
