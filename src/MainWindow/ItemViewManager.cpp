#include "ItemViewManager.h"
#include "TreeItem.h"
#include <QToolTip>
#include <QProcess>
#include <QDesktopWidget>
#include <QApplication>
#include "SettingsManager.h"
#include <QMessageBox>

ItemViewManager::ItemViewManager(Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui),
	_treeModel (dynamic_cast<TreeModel*>(ui->treeView->model ()))
{
}

QModelIndex ItemViewManager::getTreeModelIndexForSearchItem (TreeItem *item, const bool &isIndividual,
					    const IDTYPE &id, const IDTYPE &spouseIdForCurrentEntryOfFemale) const
{
	/* In the case of root item id is zero. */
	if (item->id())
	{
		if (item->isIndividual() == isIndividual &&
		    item->id() == id && (!item->isSpouseItem() ||
					 item->parentItem()->id() == spouseIdForCurrentEntryOfFemale))
			return _treeModel->index(item->myChildNumber(), 0, _treeModel->parent(item));
	}

	for (int i = 0; i < item->childCount (); i++)
	{
		TreeItem *child = item->childAt (i);
		QModelIndex index = getTreeModelIndexForSearchItem(child, isIndividual,
								   id, spouseIdForCurrentEntryOfFemale);

		if (index.isValid())
			return index;
	}

	return QModelIndex();
}

void ItemViewManager::handleAnchorClick (const QString &url) const
{
	handleAnchorClick (QUrl (url));
}

void ItemViewManager::setCurrentItem (const QModelIndex &index) const
{
	if (!index.isValid())
	{
		ui->textBrowserItemView->clear();
		return;
	}

	TreeItem *item = _treeModel->getItem (index);
	if (!item)
		return;

	ui->textBrowserItemView->setText(item->fam()->getItemViewPage(item->parentItem()->id()));
}

void ItemViewManager::handleAnchorClick (const QUrl &url) const
{
	bool searchIndividual = true;
	if (url.scheme() == "iid" || url.scheme() == "fid")
	{
		if (url.scheme() == "iid")
			searchIndividual = true;
		else
			searchIndividual = false;

		IDTYPE id = url.path().toUInt();
		IDTYPE spouseIdForCurrentEntryOfFemale = url.fragment().toUInt();

		TreeItem *rootItem = _treeModel->getItem(QModelIndex());

		QModelIndex jumpToIndex =
			getTreeModelIndexForSearchItem(rootItem, searchIndividual, id,
						       spouseIdForCurrentEntryOfFemale);
		if (!jumpToIndex.isValid())
		{
			qDebug () << "No" << (searchIndividual ? "Individual" : "Family")
					<< "entry found with: ID = " << id
					<< "and spouseIdForCurrentEntryOfFemale = "
					<< spouseIdForCurrentEntryOfFemale;
			return;
		}

		if (jumpToIndex != ui->treeView->currentIndex())
		{
			setCurrentItem(jumpToIndex);
			emit lastActiveIndexesRequireUpdation(jumpToIndex);
			emit modelViewsRequireScrolling (jumpToIndex);
		}
	}
	else if (url.scheme() == "edit")
	{
		QModelIndex currentIndex = SettingsManager::currentIndexForCurrentView ();
		if (!currentIndex.isValid())
			return;

		emit entryNeedsToBeEdited(currentIndex);
	}
	else if (url.scheme() == "image")
	{
		unsigned int id = url.path().toUInt();
		if (!id)
			return;

		Individual *individual = Individual::s_IdToIndividualHash.value(id);

		QDesktopWidget *desktopWidget = QApplication::desktop();
		QRect windowRect = desktopWidget->screenGeometry(dynamic_cast<QWidget*>(parent()));
		QPixmap *pixmap = individual->getImage();
		int imageHeight = pixmap->height();
		int imageWidth = pixmap->width();
		int screenHeight = windowRect.bottomLeft().y() - windowRect.topRight().y() -10;
		int screenWidth = windowRect.topRight().x() - windowRect.bottomLeft().x() -10;

		int dispHeight, dispWidth;
		if (imageHeight > screenHeight)
		{
			dispHeight = screenHeight;
			dispWidth = (float) dispHeight * imageWidth / imageHeight;
		}
		else
		{
			dispHeight = imageHeight;
			dispWidth = imageWidth;
		}
		if (dispWidth > screenWidth)
		{
			dispWidth = screenWidth;
			dispHeight = (float) dispWidth * dispHeight / imageWidth;
		}
		QString dispHeightString = QString(" height=\"%1\"").arg(dispHeight);
		QString dispWidthString = QString(" width=\"%1\"").arg(dispWidth);

		QString imageHTML = "<img" + dispHeightString + dispWidthString +
				    " title=\"Image of " + individual->getName() +
				    "\" src=\"" + individual->getImagePath() + "\">";
		QToolTip::showText(QPoint(5, 5), imageHTML, dynamic_cast<QWidget*>(parent()));
	}
	else
	{
		QString link = url.toString();
		if (QMessageBox::Ok == QMessageBox::question (dynamic_cast<QWidget*>(parent()), "Open Link",
							      QString ("Should %1 open the following link in web browser: %2")
							      .arg(SettingsManager::s_FamilierApplicationName).arg (link),
							      QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel))
		{
		QProcess *browserProcess = new QProcess (dynamic_cast<QWidget*>(parent()));
		QString browser ("firefox");
		QStringList arguments;

		arguments << link;

		qDebug () << "Opening " << link << " in firefox.";
		browserProcess->start(browser, arguments);
	}
}
}

void ItemViewManager::updateItem () const
{
	QModelIndex index = ui->treeView->currentIndex();
	if (!index.isValid())
		return;

	setCurrentItem(index);
}
