#ifndef EDITMENUHANDLER_H
#define EDITMENUHANDLER_H

#include <QObject>
#include <QModelIndex>
#include "TreeModel.h"
#include "FamilyTreeModel.h"
#include "IconModel.h"
#include "FamilyModel.h"
#include "EventModel.h"
#include "ui_MainWindow.h"
#include "MaleIndividual.h"
#include "FemaleIndividual.h"

class EditMenuHandler : public QObject
{
	Q_OBJECT

	enum ItemType {male, female, family};

	Ui::MainWindow			*ui;
	TreeModel			*_treeModel;
	TableModel			*_tableModel;
	IconModel			*_iconModel;
	FamilyTreeModel			*_familyTreeModel;
	FamilyModel			*_familyModel;
	EventModel			*_eventModel;

public:
	explicit EditMenuHandler(FamilyModel *familyModel, EventModel *eventModel,
				 Ui::MainWindow *_ui, QObject *parent = 0);

private:
	bool		addNewItem		(Familier *fam, int row, QModelIndex &parent) const;
	bool		addNewSpouse		(ItemType type, bool isDummy = false);
	bool		addNewSibling		(ItemType type);
	bool		addNewChild		(ItemType type);
	bool		markAsDeadOrAlive	(const bool dead);
	bool		markOrUnmarkAsDummy	(const bool dummy);

signals:
	void		documentModified	() const;
	//void		currentEntryRequiresExpansion () const;
	//Model regeneration should never happen, this should be removed in future!!
	void		modelRequiresRegeneration ();
	void		itemViewRequiresUpdate	();
	void		systemTrayNeedsToBeShown(bool);

public slots:
	void		editInline		();
	void		markAsDead		();
	void		markAsAlive		();
	void		markAsDummy		();
	void		unmarkAsDummy		();
	void		editEntry		();
	void		editEntry		(QModelIndex index);
	void		delEntry		();

	void		addNewFamily		();

	void		addNewSpouse		();
	void		addNewDummySpouse	();
	void		addNewMaleSibling	();
	void		addNewFemaleSibling	();
	void		addNewMaleChild		();
	void		addNewFemaleChild	();

	void		setDocumentTitle	();

	void		fullTextSearch		(const bool &searching) const;
	void		editPreferences		();
};

#endif // EDITMENUHANDLER_H
