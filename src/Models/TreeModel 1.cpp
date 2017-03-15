#include "TreeModel.h"
#include "TreeItem.h"
#include "MaleIndividual.h"
#include "FemaleIndividual.h"

#include <QMapIterator>
#include <QMenu>
#include <QTimer>

const QString TreeModel::s_mimetype = "application/x-familier.text.xml";

TreeModel::TreeModel (QObject *parent) : QAbstractItemModel (parent)
{
	_rootItem = new TreeItem (new Family(0), NULL);
	TreeItem::s_RootItemForFamilyTreeView = _rootItem;
}

TreeModel::TreeModel (TreeItem *rootItem, QObject *parent) : QAbstractItemModel (parent)
{
	_rootItem = rootItem;
	TreeItem::s_RootItemForFamilyTreeView = _rootItem;
}

TreeModel::~TreeModel ()
{
	if (_rootItem)
		delete _rootItem;
}

TreeItem* TreeModel::getItem (const QModelIndex &index) const
{
	if (index.isValid())
	{
		TreeItem *item = static_cast<TreeItem *> (index.internalPointer());
		if (item)
			return item;
	}
	return _rootItem;
}

TreeItem* TreeModel::getChild (const QModelIndex &index, const int row) const
{
	TreeItem *parentItem = getItem (index);

	if (!parentItem->childCount ()|| row < 0 || row >= parentItem->childCount ())
		return NULL;

	return parentItem->childAt (row);
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	TreeItem *childItem = getChild (parent, row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex TreeModel::index(TreeItem *item, int column) const
{
	if (item == _rootItem || !item)
		return QModelIndex();

	return createIndex(item->myChildNumber(), column, item);
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = getItem (index);
	return parent (childItem);
}

QModelIndex TreeModel::parent (TreeItem *childItem) const
{
	if (childItem == _rootItem)
		return QModelIndex();

	TreeItem *parentItem = childItem->parentItem();
	if (!parentItem)
		return QModelIndex();
	return createIndex (childItem->myChildNumber(), 0, parentItem);
}

bool TreeModel::hasChildren(const QModelIndex &parent) const
{
	return getItem(parent)->childCount () > 0;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	return getItem(parent)->childCount ();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED (parent)

	return Familier::columnCount() /*+ 4*/;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		/*if (section == Familier::columnCount ())
			return "X";
		if (section == Familier::columnCount () + 1)
			return "Y";
		if (section == Familier::columnCount () + 2)
			return "Width";
		if (section == Familier::columnCount () + 3)
			return "Height";*/

		return Familier::headerData(section, orientation, role);
	}

	return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	/*if (index.column () >= Familier::columnCount ())
		return Qt::ItemIsEnabled;*/

	TreeItem *item = getItem (index);

	return item->fam()->flags(index.column(), item->isSpouseItem());
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	TreeItem *item = getItem (index);
	if (item == _rootItem)
		return QVariant();

	/*if (index.column () == Familier::columnCount ())
		return item->x ();
	if (index.column () == Familier::columnCount () + 1)
		return item->y ();
	if (index.column () == Familier::columnCount () + 2)
		return item->width ();
	if (index.column () == Familier::columnCount () + 3)
		return item->height ();*/

	return item->fam()->data(index.column(), role,
				 item->isSpouseItem() ? item->parentItem()->id() : 0);
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	TreeItem *item = getItem (index);
	if (item == _rootItem)
		return false;

	if (item->fam()->setData(index.column(),
				 value, item->isSpouseItem() ? item->parentItem()->id() : 0))
	{
		/* If an individual was successfully (un)marked as dummy
		   we need to add it into individualTreeItemList of
		   TreeItem class. We are here, this itself means the
		   change was successful. Now we only need to verify
		   if dummyness was changed or not.
		 */

		if (index.column() == Familier::e_col_dummyOrBase)
			emit itemDumminessChanged(item);

		if (!item->isIndividual() && index.column() == Familier::e_col_name)
			/* We have to do this because calling beginResetModel ()
			   leaves the editing delegate in undefined state.
			 */
			QTimer::singleShot (1000, this, SLOT(adjustForFamilyNameChange()));

		emit dataChanged(index, index);
		return true;
	}

	return false;
}

/*TreeItem* TreeModel::commonParent (const TreeItem *item1, const TreeItem *item2)
{
	if (!item1 || !item2)
		return NULL;

	if (item1 == item2)
		return item1;

	QList <TreeItem *> item1ParentList;
	QList <TreeItem *> item2ParentList;
	item1ParentList << item1;
	item2ParentList << item2;

	if (!item1->parentItem () && !item2->parentItem ())
		return NULL;

	if (item1->parentItem ())
	{
		item1 = item1->parentItem ();
		item1ParentList << item1;
	}

	if (item2->parentItem ())
	{
		item2 = item2->parentItem ();
		item2ParentList << item2;
	}

	while (item1 && item2)
	{
		if (item1->isFemale ())
	}

	do
	{
		item1ParentList << item1;
		item2ParentList << item2;

		if (item1ParentList.contains (item2))
			return index (item2);

		if (item2ParentList.contains (item1))
			return index (item1);

		if (!item1->parentItem () && !item2->parentItem ())
			break;

		if (item1->parentItem ())
			item1 = item1->parentItem ();
		if (item2->parentItem ())
			item2 = item2->parentItem ();
	} while (true);

	return QModelIndex ();
}

QModelIndex TreeModel::commonParent (const QModelIndex &index1, const QModelIndex &index2)
{
	if (!index1.isValid () || !index2.isValid ())
		return QModelIndex ();

	if (index1 == index2)
		return index1;

	TreeItem *item1 = getItem (index1);
	TreeItem *item2 = getItem (index2);

	if (!item1 || !item2)
		return QModelIndex ();

	QList <TreeItem *> item1ParentList;
	QList <TreeItem *> item2ParentList;

	do
	{
		item1ParentList << item1;
		item2ParentList << item2;

		if (item1ParentList.contains (item2))
			return index (item2);

		if (item2ParentList.contains (item1))
			return index (item1);

		if (!item1->parentItem () && !item2->parentItem ())
			break;

		if (item1->parentItem ())
			item1 = item1->parentItem ();
		if (item2->parentItem ())
			item2 = item2->parentItem ();
	} while (true);

	return QModelIndex ();
}*/

bool TreeModel::insertRow(Familier *fam, bool createItemOnly, int row, const QModelIndex & parent, IDTYPE uid)
{
	if (row < 0 || row > rowCount(parent))
		return false;

	TreeItem *parentItem = getItem (parent);
/*	qDebug().nospace () << "Insert row (" << row << ") called for: " << fam->getName() << " ("
			<< fam->getId() << "). Its parent was: " << parentItem->fam->getName()
			<< " (" << parentItem->fam->getId() << ").";*/

	/* If parentItem is _rootItem then the child item has to be of type Family,
	 * in all other cases it is of type Individual.
	 */
	if (parentItem == _rootItem)
	{
		// return false if fam is not of type Family.
		if (!dynamic_cast<Family *> (fam))
			return false;
	}
	else
	{
		// return false if fam is not of type Individual.
		Individual *individual = dynamic_cast<Individual *> (fam);
		if (!individual)
			return false;

		if (parentItem->isIndividual())
		{
			/* Only a married spouse can have child items,
			 * i.e. items for females in the role of daughters can't have child items.
			 */
			if (parentItem->isFemale() && !parentItem->isSpouseItem())
				return false;

			/* A male parent item can have only female child item in the form of a spouse
			 * return false if that is not the case
			 */
			if (!parentItem->isFemale() && !individual->isFemale())
				return false;
		}
	}

	TreeItem *item = new TreeItem (fam, parentItem);

	beginInsertRows(parent, row, row);

	try
	{
		if (!createItemOnly)
		{
			/* Since we are using Family for both rootItem and its child items
			* we can use the same code for inserting into both. Fam is of
			* type Family in one case and Individual in another.
			*/
			if (parentItem == _rootItem || !parentItem->isIndividual())
			{
				Family *family = dynamic_cast<Family *> (parentItem->fam());
				if (!family)
					throw false;
				if (parentItem != _rootItem)
				{
					Individual *primaryMember = dynamic_cast<Individual *> (fam);
					if (!primaryMember->setFamily(family))
						throw false;
				}
				if (!family->appendChild (dynamic_cast<IdableWithChildren *> (fam)))
					throw false;
			}
			else
			{
				/* If parent item is female, than child item (whether male or female)
				* is offspring of parent item. Since this is adding offspring we need
				* to add this offspring to an existing Union. Note parent item is
				* always spouse item and has a parent item (her husband), even if he
				* is a dummy node.
				*/
				if (parentItem->isFemale())
				{
					Individual *father = dynamic_cast<Individual *> (parentItem->parentItem()->fam());
					Individual *mother = dynamic_cast<Individual *> (parentItem->fam());
					if (!father || !mother)
						throw false;
					Family *family = father->getFamily();
					if (!family)
						throw false;
					Union *aUnion = mother->getUnionWithSpouseID (parentItem->parentItem()->id());
					if (!aUnion)
						throw false;
					Individual *child = dynamic_cast<Individual *> (fam);
					if (!child->setParentUnion (aUnion))
						throw false;
					if (!child->setFamily(family))
						throw false;
					if (!uid)
						if (!aUnion->insertChild (row, dynamic_cast<IdableWithChildren *> (fam)))
							throw false;
				}
				/* If parent item is male, than child item can't be anything but female
				* and parent item's spouse. Since this is insertion we need to create
				* a new Union for the two.
				*/
				else
				{
					Individual *husband = dynamic_cast<Individual *> (parentItem->fam());
					Individual *wife = dynamic_cast<Individual *> (fam);
					Union *aUnion;
					if (!uid)
					{
						aUnion = new Union;
						if (!aUnion->setHusband (husband))
							throw false;
						if (!aUnion->setWife (wife))
							throw false;
					}
					else
						aUnion = Union::s_IdToUnionMap[uid];
					if (!aUnion)
						throw false;
					if (!wife->appendChild(dynamic_cast<IdableWithChildren *> (aUnion)))
						throw false;
					if (!husband->insertChild(row, dynamic_cast<IdableWithChildren *> (aUnion)))
						throw false;
				}
			}
		}

		parentItem->insertChild (row, item);
	}
	catch (bool)
	{
		endInsertRows();
		qDebug().nospace() << "Insert row (" << row << ") failed for: " << fam->getName() << " ("
				<< fam->getId() << "). Its parent was: " << parentItem->fam()->getName()
				<< " (" << parentItem->fam()->getId() << ").";
		return false;
	}

	endInsertRows();

	return true;
}

bool TreeModel::removeRow(int row, const QModelIndex & parent)
{
	TreeItem *parentItem = getItem (parent);
	TreeItem *item = getChild (parent, row);

	/* We are not deleting items recursively bailing out.*/
	if (item->childCount())
		return false;

	beginRemoveRows(parent, row, row);

	try
	{
		if (!parentItem->removeChild (item))
			throw false;

		if (parentItem == _rootItem || !parentItem->isIndividual())
		{
			Family *family = dynamic_cast<Family *> (parentItem->fam());
			if (!family->removeChildAt (row))
				throw false;

			if (item->isIndividual())
			{
				Individual *individual = dynamic_cast<Individual*>(item->fam());
				delete item;
				if (individual->getRefCount () == 0)
					delete individual;
				else
					individual->setFamily (NULL);
			}
			else
			{
				Family *family = dynamic_cast<Family*>(item->fam());
				delete item;
				delete family;
				QTimer::singleShot (1000, this, SLOT(adjustForFamilyNameChange()));
			}
		}
		else
		{
			/* If parent is female item she is mother and we have to
			   remove this particular child from union. Child item has
			   to be deleted as well. It has no references elsewhere,
			   since we are not allowing deletion of items with children
			   anyway. A male is referenced elsewhere in the tree only by
			   his spouse. In this case there is no spouse since tree is
			   empty. So we are not required to do anything more than
			   deleting the individual;
			 */
			if (parentItem->isFemale())
			{
				Individual *mother = dynamic_cast<Individual *> (parentItem->fam());
				Union *aUnion = mother->getUnionWithSpouseID (parentItem->parentItem()->id());
				if (!aUnion)
					throw false;
				if (!aUnion->removeChildAt(row))
					throw false;

				/* Note female can item can be spouse elsewhere.*/
				Individual *individual = dynamic_cast<Individual*>(item->fam());

				delete item;

				/* If individual is not being refered we delete the
				   individual, else we remove its family, becuase
				   he/she is not a member of this family anymore.
				 */
				if (individual->getRefCount() == 0)
					delete individual;
				else
				{
					individual->setFamily (NULL);
					individual->setParentUnion (NULL);
				}
			}
			/* If parent item is male, he is husband and we have to
			   remove this particular wife item and delete the union.
			   If wife item is not referenced anywhere else its
			   individual has to be removed as well.
			 */
			else
			{
				if (!item->isFemale())
					throw false;

				Individual *husband = dynamic_cast<Individual *> (parentItem->fam());
				Individual *wife = dynamic_cast<Individual *> (item->fam());
				if (!husband || !wife)
					throw false;

				Union *aUnion = husband->getUnionWithSpouseID (item->id());
				if (!aUnion)
					throw false;

				// Removing union from husband
				if (!husband->removeChildAt(row))
					throw false;

				/* Removing union from wife. Note this is to be
				   done as wife's individual might not be removed
				   if its reference count is non zero.
				 */
				if (!wife->removeChild(dynamic_cast<IdableWithChildren *>(aUnion)))
					throw false;

				delete item;
				if (wife->getRefCount() == 0)
					delete wife;

				delete aUnion;
			}
		}
	}
	catch (bool)
	{
		endRemoveRows ();
		qDebug().nospace() << "Remove row (" << row << ") failed for: "
				<< item->fam()->getName () << " (" << item->id () << ")."
				<< "Its parent was: " << parentItem->fam()->getName()
				<< " (" << parentItem->fam()->getId() << ").";
		return false;
	}

	endRemoveRows();
	return true;
}

Qt::DropActions TreeModel::supportedDragActions() const
{
	return Qt::MoveAction;
}

Qt::DropActions TreeModel::supportedDropActions() const
{
	 return Qt::MoveAction;
}

QStringList TreeModel::mimeTypes() const
{
	QStringList types;
	types << s_mimetype;
	return types;
}

QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();

	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (QModelIndex index, indexes)
	{
		if (index.isValid())
		{
			TreeItem *item = getItem(index);
			TreeItem *parentItem = getItem(index.parent());
			stream << reinterpret_cast<qint32>(item) << reinterpret_cast<qint32>(parentItem);
		}
	}

	mimeData->setData(s_mimetype, encodedData);
	return mimeData;
}

bool TreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
			     int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column)
	Q_UNUSED(row)

	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat(s_mimetype))
		return false;

	QByteArray encodeData = data->data(s_mimetype);
	QDataStream stream(&encodeData, QIODevice::ReadOnly);

	QModelIndex destinationIndex;
	if (row == -1)
	{
		//add as child
		destinationIndex = parent;
	}
	else
	{
		//add as sibling
		destinationIndex = index(row, column, parent);
	}

	TreeItem *destinationItem = getItem(destinationIndex);
	TreeItem *parentOfDestinationItem = destinationItem->parentItem();

	qint32 ptr;
	stream >> ptr;
	TreeItem *sourceItem = reinterpret_cast<TreeItem*>(ptr);
	stream >> ptr;
	TreeItem *parentOfSourceItem = reinterpret_cast<TreeItem*>(ptr);

	if (!stream.atEnd() || !sourceItem || !parentOfSourceItem)
		return false;

	bool dropAsChildIsValid = false;
	bool dropAsAdoptedChildIsValid = false;
	bool dropAsSiblingIsValid = false;
	bool dropAsSpouseIsValid = false;
	bool dropAsWifeInLawIsValid = false;

	/* We do not have any drag/drop for root item and sourceItem
	   has to be individual and non-dummy.
	 */
	if (destinationItem == _rootItem || !sourceItem->isIndividual () || sourceItem->isDummy ())
		return false;

	if (destinationItem->isIndividual ())
	{
		/* Since parent is valid, parent is not root item.
		   1) Destination item has to be individual.
		   2) So source can not be family.
		   3) If destination is FEMALE and is in role of DAUGHTER than
		   only drop as SIBLING is allowed.
		   4) If destination is FEMALE and in the role of SPOUSE than
		   drop as CHILD and if source is FEMALE drop as SIBLING SPOUSE is allowed.
		   5) If destination is Male drop as SIBLING, and if source
		   is female drop as SPOUSE is allowed.
		 */

		dropAsSiblingIsValid = true;
		if (destinationItem->isFemale())
		{
			if (destinationItem->isSpouseItem())
			{
				/* Not needed instead dropAsWifeInLawIsValid will
				   be set if required.
				 */
				dropAsSiblingIsValid = false;
				/* Nobody can be her own mother! */
				if (destinationItem->id () != sourceItem->id ())
				{
					dropAsChildIsValid = true;
					dropAsAdoptedChildIsValid = true;
				}

				/* The sourceItem can be only female, also we do
				   not want same two individuals having two
				   different unions.
				 */
				if ( sourceItem->isFemale() &&
				     !dynamic_cast<Individual*>(parentOfDestinationItem->fam())->isSpouse (sourceItem->id ()))
					dropAsWifeInLawIsValid = true;
			}
		}
		else
		{
			if (sourceItem->isFemale() &&
			    !dynamic_cast<Individual*>(destinationItem->fam())->isSpouse (sourceItem->id ()))
				dropAsSpouseIsValid = true;
		}
	}
	else
	{
		/* Since parent is not valid, parent is root item.
		   1) Destination item has to be family.
		   2) Source item has to be individual, and only CHILD action
		   is allowed.
		   3) Source item can not be family because we sort families,
		   so SIBLING doesn't make sense.
		 */

		dropAsChildIsValid = true;
	}

	QMenu pasteMenu;
	pasteMenu.addAction (QIcon(QPixmap(":images/help_about.png")),
						   QString ("Dropping %1 on %2")
						   .arg (sourceItem->fam()->getName ())
						   .arg(destinationItem->fam()->getName ()));
	QAction *dropAsChildAction = pasteMenu.addAction(QIcon(QPixmap(":images/copy_child.png")),
							 tr("Cut source and drop as child"));
	QAction *dropAsAdoptedChildAction = pasteMenu.addAction (QIcon(QPixmap(":images/copy_child.png")),
								 tr ("Copy source and drop as adopted child"));
	QAction *dropAsSiblingAction = pasteMenu.addAction(QIcon(QPixmap(":images/copy_sibling.png")),
							   tr("Cut source and drop as sibling"));
	QAction *dropAsSpouseAction = pasteMenu.addAction(QIcon(QPixmap(":images/copy_spouse.png")),
							  tr("Copy source and drop as spouse"));
	QAction *dropAsWifeInLawAction = pasteMenu.addAction(QIcon(QPixmap(":images/copy_spouse.png")),
								 tr("Copy source and drop as wife-in-law"));
	pasteMenu.addAction(QIcon(QPixmap(":images/exit.png")), tr("Do nothing"));

	if (!dropAsChildIsValid)
		dropAsChildAction->setDisabled (true);
	if (!dropAsAdoptedChildIsValid)
		dropAsAdoptedChildAction->setDisabled (true);
	if (!dropAsSiblingIsValid)
		dropAsSiblingAction->setDisabled (true);
	if (!dropAsSpouseIsValid)
		dropAsSpouseAction->setDisabled (true);
	if (!dropAsWifeInLawIsValid)
		dropAsWifeInLawAction->setDisabled (true);

	QAction *selectedItem = pasteMenu.exec(QCursor::pos());

	/* Note we have disabled all non-applicable actions. We do
	   not need to verify them again. We will assume that we are
	   fine in that department.
	 */

	/* Here destinationItem is male and sourceItem is female.
	   We are not going to remove source item.
	 */
	if (selectedItem == dropAsSpouseAction)
	{
		if (insertRow(sourceItem->fam(), false, 0, destinationIndex))
		{
			emit rowInserted();
			emit expandAssociatedTree();
			return true;
		}

	}
	/* Here destination is married female and source is female.
	   We are not going to remove source item.
	 */
	else if (selectedItem == dropAsWifeInLawAction)
	{
		if (insertRow(sourceItem->fam(), false, destinationItem->myChildNumber() + 1,
			      this->parent(destinationIndex)))
		{
			emit rowInserted();
			emit expandAssociatedTree();
			return true;
		}
	}
	/* Here source can be either male or female. But destination
	   has to be female individual in the role of spouse.
	 */
	else if (selectedItem == dropAsAdoptedChildAction)
	{
		Individual *individualToBeAdopted = dynamic_cast<Individual *>(sourceItem->fam ());
		Individual *newIndividual = individualToBeAdopted->adoptAndReturnNewIndividual ();

		if (insertRow(newIndividual, false, 0, destinationIndex))
		{
			emit rowInserted ();
			emit expandAssociatedTree ();
			return true;
		}
	}
	/* Here both destination and source can be either male or
	   female. We have to add source into parent of destination.
	   Source item will be removed if it is not married female.
	 */
	else if (selectedItem == dropAsSiblingAction)
	{
		beginResetModel();

		TreeItem *itemToBeMoved = NULL;
		TreeItem *parentOfItemToBeMoved = NULL;
		if (!sourceItem->isSpouseItem())
		{
			itemToBeMoved = sourceItem;
			parentOfItemToBeMoved = parentOfSourceItem;
		}
		else
		{
			foreach (TreeItem *tempItem, TreeItem::individualTreeItemList)
			{
				if (tempItem->id() == sourceItem->id() && !tempItem->isSpouseItem())
				{
					itemToBeMoved = tempItem;
					parentOfItemToBeMoved = tempItem->parentItem();
				}
			}
		}

		Individual *itemToBeMovedIndividual = NULL;
		if (itemToBeMoved)
		{
			parentOfItemToBeMoved->removeChild (itemToBeMoved);
			IDTYPE chid = itemToBeMoved->id();
			if (parentOfItemToBeMoved->isIndividual ())
			{
				/* This individual is going to be the mother. */
				Individual *parentOfItemToBeMovedIndividual =
						dynamic_cast<Individual*>(parentOfItemToBeMoved->fam());
				IDTYPE spid = parentOfItemToBeMoved->parentItem()->id();
				Union *aUnion = parentOfItemToBeMovedIndividual->getUnionWithSpouseID(spid);
				aUnion->removeChild(chid);
			}
			else
			{
				Family *parentOfItemToBeMovedFamily =
						dynamic_cast<Family*>(parentOfItemToBeMoved->fam());
				parentOfItemToBeMovedFamily->removeChild (chid);
			}

			itemToBeMovedIndividual = dynamic_cast<Individual*>(itemToBeMoved->fam());
			itemToBeMovedIndividual->setParentUnion (NULL);
			itemToBeMovedIndividual->setFamily(NULL);
		}
		/* The individual exists only as spouse item. Se we have to create
		   the new item.
		 */
		else
		{
			itemToBeMovedIndividual = dynamic_cast<Individual*>(sourceItem->fam());
			itemToBeMoved = new TreeItem (itemToBeMovedIndividual, parentOfDestinationItem);
		}

		if (parentOfDestinationItem->isIndividual ())
		{
			Individual *destinationIndividual =
					dynamic_cast<Individual*>(parentOfDestinationItem->fam());
			Individual *parentOfDestinationIndividual =
					dynamic_cast<Individual*>(parentOfDestinationItem->parentItem()->fam());
			IDTYPE spid = parentOfDestinationItem->parentItem()->id();
			Union *aUnion = destinationIndividual->getUnionWithSpouseID(spid);
			aUnion->insertChild(destinationItem->myChildNumber() + 1, itemToBeMovedIndividual);
			itemToBeMovedIndividual->setParentUnion (aUnion);
			itemToBeMovedIndividual->setFamily(parentOfDestinationIndividual->getFamily());
		}
		else
		{
			Family *family = dynamic_cast<Family*>(parentOfDestinationItem->fam());
			family->insertChild (destinationItem->myChildNumber () + 1, itemToBeMovedIndividual);
			itemToBeMovedIndividual->setFamily (family);
		}
		parentOfDestinationItem->insertChild (destinationItem->myChildNumber() + 1, itemToBeMoved);

		endResetModel();

		QTimer::singleShot (1000, this, SLOT(adjustForModelReset()));
		return true;
	}
	/* Here destination has to be married female or Family.
	   Source can be either male or female. Source item will
	   be removed if it is not married female.
	 */
	else if (selectedItem == dropAsChildAction)
	{
		beginResetModel();

		TreeItem *itemToBeMoved = NULL;
		TreeItem *parentOfItemToBeMoved = NULL;
		if (!sourceItem->isSpouseItem())
		{
			itemToBeMoved = sourceItem;
			parentOfItemToBeMoved = parentOfSourceItem;
		}
		else
		{
			foreach (TreeItem *tempItem, TreeItem::individualTreeItemList)
			{
				if (tempItem->id() == sourceItem->id() && !tempItem->isSpouseItem())
				{
					itemToBeMoved = tempItem;
					parentOfItemToBeMoved = tempItem->parentItem();
				}
			}
		}

		Individual *itemToBeMovedIndividual = NULL;
		if (itemToBeMoved)
		{
			parentOfItemToBeMoved->removeChild (itemToBeMoved);
			IDTYPE chid = itemToBeMoved->id();
			if (parentOfItemToBeMoved->isIndividual ())
			{
				/* This individual is going to be the mother. */
				Individual *parentOfItemToBeMovedIndividual =
						dynamic_cast<Individual*>(parentOfItemToBeMoved->fam());
				IDTYPE spid = parentOfItemToBeMoved->parentItem()->id();
				Union *aUnion = parentOfItemToBeMovedIndividual->getUnionWithSpouseID(spid);
				aUnion->removeChild(chid);
			}
			else
			{
				Family *parentOfItemToBeMovedFamily =
						dynamic_cast<Family*>(parentOfItemToBeMoved->fam());
				parentOfItemToBeMovedFamily->removeChild (chid);
			}

			itemToBeMovedIndividual = dynamic_cast<Individual*>(itemToBeMoved->fam());
			itemToBeMovedIndividual->setParentUnion (NULL);
			itemToBeMovedIndividual->setFamily(NULL);
		}
		/* The individual exists only as spouse item. Se we have to create
		   the new item.
		 */
		else
		{
			itemToBeMovedIndividual = dynamic_cast<Individual*>(sourceItem->fam());
			itemToBeMoved = new TreeItem (itemToBeMovedIndividual, destinationItem);
		}

		if (destinationItem->isIndividual ())
		{
			Individual *destinationIndividual = dynamic_cast<Individual*>(destinationItem->fam());
			Individual *parentOfDestinationIndividual =
					dynamic_cast<Individual*>(destinationItem->parentItem()->fam());
			IDTYPE spid = destinationItem->parentItem()->id();
			Union *aUnion = destinationIndividual->getUnionWithSpouseID(spid);
			aUnion->insertChild(0, itemToBeMovedIndividual);
			itemToBeMovedIndividual->setParentUnion (aUnion);
			itemToBeMovedIndividual->setFamily(parentOfDestinationIndividual->getFamily());
		}
		else
		{
			Family *destinationFamily = dynamic_cast<Family*>(destinationItem->fam());
			destinationFamily->insertChild (0, itemToBeMovedIndividual);
			itemToBeMovedIndividual->setFamily (destinationFamily);
		}

		destinationItem->insertChild (0, itemToBeMoved);

		endResetModel();

		QTimer::singleShot (1000, this, SLOT(adjustForModelReset()));
		return true;
	}

	return false;
}

void TreeModel::clear()
{
	beginResetModel();
	_rootItem->clear();
	_rootItem->fam()->clearChildren();
	TreeItem::s_RootItemForFamilyTreeView = _rootItem;
	endResetModel();
}

void TreeModel::adjustForModelReset ()
{
	beginResetModel ();
	endResetModel ();
	emit expandAssociatedTree();
}

void TreeModel::adjustForFamilyNameChange ()
{
	beginResetModel();

	int row = 0;
	foreach (Family *family, Family::s_NameToBaseFamilyMap)
	{
		if (family->getId() != _rootItem->childAt (row)->id())
		{
			int drow = 0;
			for (drow = row + 1; drow < _rootItem->childCount(); drow++)
				if (family->getId() == _rootItem->childAt(drow)->id())
					break;
			TreeItem *temp1 = _rootItem->childAt(row);
			TreeItem *temp2 = _rootItem->childAt(drow);
			_rootItem->replaceChild (row, temp2);
			_rootItem->replaceChild (drow, temp1);
		}
		row ++;
	}

	foreach (Family *family, Family::s_NameToNonBaseFamilyMap)
	{
		if (family->getId() != _rootItem->childAt(row)->id())
		{
			int drow = 0;
			for (drow = row + 1; drow < _rootItem->childCount(); drow++)
				if (family->getId() == _rootItem->childAt(drow)->id())
				{
					TreeItem *temp1 = _rootItem->childAt(row);
					TreeItem *temp2 = _rootItem->childAt(drow);
					_rootItem->replaceChild (row, temp2);
					_rootItem->replaceChild (drow, temp1);
					break;
				}
		}
		row ++;
	}

	endResetModel();

	emit expandAssociatedTree();
}
