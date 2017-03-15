#ifndef CLASSEDITINDIVIDUAL_H
#define CLASSEDITINDIVIDUAL_H

#include "EditDialog.h"
#include "ui_EditIndividual.h"
#include "Individual.h"
#include "TreeModel.h"
#include "IconModel.h"
#include "FamilyModel.h"
#include "EventModel.h"
#include "EventModelForComboBox.h"
#include "RichTextEditor.h"

#include <QtGui/QDialog>

class EditIndividualDialog : public EditDialog
{
	Q_OBJECT

	Ui::EditIndividualDialog *	ui;

	TreeModel *			_treeModel;
	TreeItem *			_treeItem;
	IconModel *			_iconModel;
	FamilyModel *			_familyModel;
	EventModel *			_eventModel;
	EventModelForComboBox *		_eventModelForComboBox;
	Individual *			_individual;

	QList <QComboBox *>		_marriageComboBoxes;
	QList <QComboBox *>		_divorceComboBoxes;
	RichTextEditor *		_richTextEditor;

public:
	EditIndividualDialog(TreeModel *treeModel, TreeItem *treeItem,
			     IconModel *iconModel, FamilyModel *familyModel,
			     EventModel *eventModel, QWidget *parent = 0);
	~EditIndividualDialog();

private:
	void		setupUiPersonalInformationPage	();
	void		updateIndexForComboBox		(QComboBox *comboBox, Event *event);
	void		setupUiSetComboBoxForEvents	(QComboBox *comboBox, Event *event);
	void		setupUiEventLogPage		();
	void		setupUiUnionsPage		();
	void		setupUiNotesPage		();
	virtual void	setupUi				();
	void		setIndividualImage		(QString imagePath) const;

public:
	virtual bool	save				();

private slots:
	virtual void	setEdited			();
	void		loadImage			();
	void		setVillageInherited		(bool inherited);
	void		setAddressInherited		(bool inherited);
	void		addEvent			();
	void		delEvent			();
	void		updateIndicesForComboBoxes	();

signals:
	void		imageEdited			(IDTYPE);
};

#endif // CLASSEDITINDIVIDUAL_H
