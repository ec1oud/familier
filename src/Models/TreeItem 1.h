#ifndef CLASSTREEITEM_H
#define CLASSTREEITEM_H

#include "Familier.h"
#include <QHash>
#include <QList>

class TreeItem
{
	/* List automatically maintains a list of all tree items.
	 * This will be useful for List and Icon view models.
	 */
public:
	static QList<TreeItem *> familyTreeItemList;
	static QHash<IDTYPE, TreeItem*> idToFamilyTreeItemHash;
	static QList<TreeItem *> individualTreeItemList;
	static QHash<IDTYPE, TreeItem*> idToIndividualTreeItemHash;
	static TreeItem * s_RootItemForFamilyTreeView;

private:
	bool			_copyConstructed;
	Familier *		_fam;
	TreeItem *		_parentItem;
	QList <TreeItem *>	_childItems;

	bool			_isIndividual;
	bool			_isSpouseItem;

public:
	TreeItem (Familier *fam, TreeItem *parentItem);
	TreeItem (const TreeItem &treeItem);
	~TreeItem ();

	Familier *	fam			() const { return _fam; }
	TreeItem *	parentItem		() const { return _parentItem; }
	TreeItem *	leftMostChild		() const;
	TreeItem *	rightMostChild		() const;
	TreeItem *	leftChildFrom		(const TreeItem *refChild) const;
	TreeItem *	rightChildFrom		(const TreeItem *refChild) const;

	IDTYPE		id			() const;
	bool		isRoot			() const { return _parentItem == NULL; }
	bool		isIndividual		() const { return _isIndividual; }
	bool		isFamily		() const { return !_isIndividual; }
	bool		isFemale		() const;
	bool		isSpouseItem		() const { return _isSpouseItem; }
	bool		isDead			() const;
	bool		isDummy			() const;
	bool		hasImage		() const;
	QList <TreeItem*>
			getAllItemsForTheSameIndividual () const;

	int		myChildNumber		() const;

	bool		hasChild		() const;
	int		childCount		() const;
	bool		appendChild		(TreeItem *child);
	bool		removeChild		(TreeItem *child);
	bool		insertChild		(const long chidx, TreeItem *child);
	void		replaceChild		(int chidx, TreeItem *child);
	TreeItem*	childAt			(const long chidx) const;
	int		indexOfChild		(const TreeItem *child) const;

	void		clear			();
};

bool			operator <		(const TreeItem &item1, const TreeItem &item2);
bool			operator >		(const TreeItem &item1, const TreeItem &item2);
bool			operator ==		(const TreeItem &item1, const TreeItem &item2);

#endif // CLASSTREEITEM_H
