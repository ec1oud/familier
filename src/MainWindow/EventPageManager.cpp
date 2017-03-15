#include "EventPageManager.h"
#include <QMessageBox>

EventPageManager::EventPageManager(EventModel *eventModel, Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui), _eventModel(eventModel)
{
	connect (ui->toolButtonEventAdd, SIGNAL(clicked()), this, SLOT(addEvent()));
	connect (ui->toolButtonEventDelete, SIGNAL(clicked()), this, SLOT(delEvent()));
}

void EventPageManager::addEvent ()
{
	int row = _eventModel->rowCount(QModelIndex());
	if (!_eventModel->insertRow(row, QModelIndex()))
	{
		QMessageBox::warning(dynamic_cast<QWidget*>(parent ()), "Error", "Adding of new event failed.");
		return;
	}

	QModelIndex newIndex = ui->tableViewForEvents->model ()->index (row, 0, QModelIndex());
	ui->tableViewForEvents->scrollTo(newIndex);
	ui->tableViewForEvents->edit(newIndex);

	emit documentModified ();
}

void EventPageManager::delEvent ()
{
	QModelIndex currentIndex = ui->tableViewForEvents->currentIndex();
	if (!currentIndex.isValid())
	{
		QMessageBox::warning(dynamic_cast<QWidget*>(parent ()), "Error", "No event selected");
		return;
	}

	if (QMessageBox::question(dynamic_cast<QWidget*>(parent ()), "Delete event",
				  "Are you sure you want to delete selected event?",
				  QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		if (_eventModel->removeRow(currentIndex.row(), QModelIndex()))
			emit documentModified ();
	}
}
