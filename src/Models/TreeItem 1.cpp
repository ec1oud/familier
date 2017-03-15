#include "TreeItem.h"

#include "Family.h"
#include "Individual.h"

QList<TreeItem *> TreeItem::familyTreeItemList;
QHash<IDTYPE, TreeItem*> TreeItem::idToFamilyTreeItemHash;
QList<TreeItem *> TreeItem::individualTreeItemList;
QHash<IDTYPE, TreeItem*> TreeItem::idToIndividualTreeItemHash;
TreeItem* TreeItem::s_RootItemForFamilyTreeView = NULL;

TreeItem::TreeItem (Familier *fam, TreeItem *parentItem)
	: _copyConstructed (false), _fam (fam), _parentItem (parentItem)
{
	/* If there is no parent item, it is root item. */
	if (!parentItem)
	{
		_isIndividual = false;
		_isSpouseItem = false;
	}
	else
	{
		fam->refCountPP();

		if (dynamic_cast<Family *>(fam))
		{
			_isIndividual = false;
			_isSpouseItem = false;
			familyTreeItemList.append(this);
			idToFamilyTreeItemHash [fam->getId ()] = this;
		}
		else if (dynamic_cast<Individual *>(fam))
		{
			_isIndividual = true;

			/* We have already checked for female in the role of daughter items,
			* in all other cases male items will have female child items (of their spouses)
			* female items can have both male and female child items.
			*/
			/* An item will be female in role of spouse if and only if parent item is male */
			_isSpouseItem = parentItem->isIndividual() && !parentItem->isFemale ();

			if (!isDummy())
			{
				bool found = false;
				int index = -1;
				foreach (TreeItem *item, individualTreeItemList)
				{
					index ++;
					if (item->id() == id())
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					individualTreeItemList.append (this);
					idToIndividualTreeItemHash [fam->getId ()] = this;
				}
				else
				{
					if (!isSpouseItem())
					{
						individualTreeItemList.replace(index, this);
						idToIndividualTreeItemHash [fam->getId ()] = this;
					}
				}
			}
		}
	}
}

TreeItem::TreeItem (const TreeItem &treeItem)
{
	_copyConstructed = true;
	_fam = treeItem._fam;
	_parentItem = treeItem._parentItem;
	_childItems = treeItem._childItems;
	_isIndividual = treeItem._isIndividual;
	_isSpouseItem = treeItem._isSpouseItem;
}

TreeItem::~TreeItem ()
{
	if (!_copyConstructed)
	{
		foreach (TreeItem *child, _childItems)
			delete child;

		_fam->refCountMM();

		if (_isIndividual)
		{
			individualTreeItemList.removeOne (this);
			idToIndividualTreeItemHash.remove (id ());
		}
		else
		{
			familyTreeItemList.removeOne (this);
			idToFamilyTreeItemHash.remove (id ());
		}

	}
}

TreeItem* TreeItem::leftMostChild () const
{
	if (hasChild ())
		return _childItems.at(0);

	return NULL;
}

TreeItem* TreeItem::rightMostChild () const
{
	if (hasChild ())
		return _childItems.at (childCount () - 1);

	return NULL;
}

TreeItem* TreeItem::leftChildFrom (const TreeItem *refChild) const
{
	int childNumber = _childItems.indexOf (const_cast<TreeItem*>(refChild));
	if (childNumber <= 0)
		return NULL;

	return _childItems.at (childNumber - 1);
}

TreeItem* TreeItem::rightChildFrom (const TreeItem *refChild) const
{
	int childNumber = _childItems.indexOf (const_cast<TreeItem*>(refChild));
	if (childNumber < 0 || childNumber >= _childItems.count () - 1)
		return NULL;

	return _childItems.at (childNumber + 1);
}

IDTYPE TreeItem::id() const
{
	return _fam->getId();
}

bool TreeItem::isFemale() const
{
	if (_isIndividual)
		return dynamic_cast<Individual *>(_fam)->isFemale();
	return false;
}

bool TreeItem::isDead() const
{
	if (_isIndividual)
		return dynamic_cast<Individual *>(_fam)->isDead();
	return false;
}

bool TreeItem::isDummy() const
{
	if (_isIndividual)
		return dynamic_cast<Individual *>(_fam)->isDummy();
	return false;
}

bool TreeItem::hasImage () const
{
	if (_isIndividual)
		return dynamic_cast<Individual *>(_fam)->hasImage ();
	return false;
}

/*QList <TreeItem *> TreeItem::getAllItemsForTheSameIndividual () const
{
	QList <TreeItem *> itemList;
	itemList << this;

	if (isFamily () || isFemale ())
		return itemList;

	Individual *individual = fam ();
	if (!individual->hasSpouse ())
		return itemList;
}*/

int TreeItem::myChildNumber() const
{
	if (_parentItem)
		return _parentItem->indexOfChild (const_cast<TreeItem*>(this));

	return 0;
}

bool TreeItem::hasChild () const
{
	return _childItems.count () > 0;
}

int TreeItem::childCount () const
{
	return _childItems.count ();
}

bool TreeItem::appendChild (TreeItem *child)
{
	return insertChild (childCount (), child);
}

bool TreeItem::removeChild (TreeItem *child)
{
	if (_childItems.removeOne (child))
	{
		child->_parentItem = NULL;
		return true;
	}

	return false;
}

bool TreeItem::insertChild (const long chidx, TreeItem *child)
{
	if (chidx < 0 || chidx > childCount ())
		return false;

	_childItems.insert (chidx, child);
	child->_parentItem = this;
	return true;
}

void TreeItem::replaceChild (int chidx, TreeItem *child)
{
	_childItems.replace (chidx, child);
	child->_parentItem = this;
}

TreeItem* TreeItem::childAt (const long chidx) const
{
	return _childItems.at (chidx);
}

int TreeItem::indexOfChild (const TreeItem *child) const
{
	return _childItems.indexOf (const_cast<TreeItem*>(child));
}

void TreeItem::clear()
{
	foreach (TreeItem *child, _childItems)
		delete child;

	_childItems.clear();
}

bool operator < (const TreeItem &item1, const TreeItem &item2)
{
	if (item1.isIndividual () && item2.isIndividual ())
	{
		Individual *i1 = dynamic_cast<Individual*>(item1.fam ());
		Individual *i2 = dynamic_cast<Individual*>(item2.fam ());
		if (i1->getFullName () == i2->getFullName ())
		{
			if (i1->getFather () && i2->getFather ())
			{
				Individual *f1 = dynamic_cast<Individual*>(i1->getFather ());
				Individual *f2 = dynamic_cast<Individual*>(i2->getFather ());
				if (f1->getName () != f2->getName ())
					return f1->getName () < f2->getName ();
			}
			if (i1->getBirthEvent () && i2->getBirthEvent () &&
			    i1->getBirthEvent () != i2->getBirthEvent ())
			{
				Event e1 = *(i1->getBirthEvent ());
				Event e2 = *(i2->getBirthEvent ());
				return e1 < e2;
			}
			return i1->getId () < i2->getId ();
		}
		return item1.fam()->getName() < item2.fam()->getName();
	}

	return item1.fam ()->getName () < item2.fam ()->getName ();
}

bool operator > (const TreeItem &item1, const TreeItem &item2)
{
	if (item1 == item2)
		return false;

	return !(item1 < item2);
}

bool operator == (const TreeItem &item1, const TreeItem &item2)
{
	return item1.isIndividual () == item2.isIndividual () &&
			item1.id () == item2.id ();
}
