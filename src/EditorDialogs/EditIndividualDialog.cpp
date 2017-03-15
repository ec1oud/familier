#include "EditIndividualDialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include "Family.h"
#include "FemaleIndividual.h"
#include <QPushButton>
#include "SettingsManager.h"
#include "RichTextEditor.h"
#include "RichTextEditorDelegate.h"
#include "Utilities.h"

EditIndividualDialog::EditIndividualDialog (TreeModel *treeModel, TreeItem *treeItem,
					    IconModel *iconModel, FamilyModel *familyModel,
					    EventModel *eventModel, QWidget *parent)
	: EditDialog (dynamic_cast<Individual*>(treeItem->fam ())->getAliases (), parent),
	ui (new Ui::EditIndividualDialog), _treeModel (treeModel), _treeItem (treeItem),
	_iconModel(iconModel), _familyModel (familyModel), _eventModel (eventModel),
	_individual (dynamic_cast<Individual*>(treeItem->fam ()))
{
	_eventModelForComboBox = new EventModelForComboBox;
	_eventModelForComboBox->setSourceModel(_eventModel);
	ui->setupUi(this);
	setupUi();
}

EditIndividualDialog::~EditIndividualDialog()
{
	delete ui;
}

void EditIndividualDialog::setupUiPersonalInformationPage ()
{
	QModelIndex index = _treeModel->index (_treeItem, Familier::e_col_name);
	ui->lineEditName->setText(_treeModel->data (index, Qt::EditRole).toString ());
	connect (ui->lineEditName, SIGNAL(textChanged(QString)), this, SLOT(setEdited()));

	if (_individual->getFamily())
		ui->labelFamilyNameValue->setText(_individual->getFamily()->getName());

	_aliasesListView = ui->listViewAliases;
	ui->listViewAliases->setModel(_aliasesModel);
	connect (ui->toolButtonAliasesAdd, SIGNAL(clicked()), this, SLOT(addAlias()));
	connect (ui->toolButtonAliasesDel, SIGNAL(clicked()), this, SLOT(delAlias()));
	connect (ui->toolButtonAliasesUp, SIGNAL(clicked()), this, SLOT(movAliasUp()));
	connect (ui->toolButtonAliasesDown, SIGNAL(clicked()), this, SLOT(movAliasDn()));

	ui->labelSiblingsValue->setText(_individual->getListOfAnchors(Individual::siblings, false, false));

	if (_individual->isFemale())
		ui->labelSexValue->setText("Female");
	else
		ui->labelSexValue->setText("Male");

	index = _treeModel->index (_treeItem, Familier::e_col_dead);
	if (_treeModel->data (index, Qt::EditRole).toBool ())
		ui->radioButtonDeadYes->setChecked(true);
	else
		ui->radioButtonDeadNo->setChecked(true);
	connect (ui->radioButtonDeadYes, SIGNAL(toggled(bool)), this, SLOT(setEdited()));
	connect (ui->radioButtonDeadNo, SIGNAL(toggled(bool)), this, SLOT(setEdited()));

	if (_individual->getFather())
		ui->labelFatherValue->setText(_individual->getFather()->getName());
	if (_individual->getMother())
		ui->labelMotherValue->setText(_individual->getMother()->getName());

	QString imagePath = _individual->getImagePath();
	setIndividualImage (imagePath);
	if (imagePath.at(0) != ':')
	{
		imagePath = imagePath.replace(SettingsManager::s_DocumentDirectoryPath + "/", "");
		ui->lineEditImagePath->setText(imagePath);
	}
	connect(ui->lineEditImagePath, SIGNAL(textChanged(QString)), this, SLOT(setEdited()));
	connect(ui->toolButtonImagePath, SIGNAL(clicked()), this, SLOT(loadImage()));

	ui->lineEditFacebookID->setText (_individual->getFacebookID ());
	connect (ui->lineEditFacebookID, SIGNAL(textChanged(QString)), this, SLOT(setEdited()));

	if (_individual->isFemale())
		ui->labelMaidenNameValue->setText(dynamic_cast<FemaleIndividual*>(_individual)->getMaidenName());
	else
	{
		ui->labelMaidenName->setHidden(true);
		ui->labelMaidenNameValue->setHidden(true);
	}

	ui->lineEditVillage->setText(_individual->getVillage());
	if (_individual->isVillageInherited())
		setVillageInherited(true);
	else
		setVillageInherited(false);
	connect(ui->checkBoxVillage, SIGNAL(toggled(bool)), this, SLOT(setEdited()));
	connect(ui->checkBoxVillage, SIGNAL(toggled(bool)), this, SLOT(setVillageInherited(bool)));
	connect(ui->lineEditVillage, SIGNAL(textChanged(QString)), this, SLOT(setEdited()));

	ui->lineEditAddress->setText(_individual->getAddress());
	if (_individual->isAddressInherited())
		setAddressInherited(true);
	else
		setAddressInherited(false);
	connect(ui->checkBoxAddress, SIGNAL(toggled(bool)), this, SLOT(setEdited()));
	connect(ui->checkBoxAddress, SIGNAL(toggled(bool)), this, SLOT(setAddressInherited(bool)));
	connect(ui->lineEditAddress, SIGNAL(textChanged(QString)), this, SLOT(setEdited()));

	ui->labelCreatedOnValue->setText(_individual->getCreationTime().toString());
	ui->labelLastModifiedValue->setText(_individual->getModificationTime().toString());
	ui->lcdNumberModificationCount->display(static_cast<int>(_individual->getModificationCount()));
}

void EditIndividualDialog::updateIndexForComboBox (QComboBox *comboBox, Event *event)
{
	if (event)
	{
		IDTYPE id = event->getId();
		comboBox->setCurrentIndex(_eventModel->getIndexFromId(id) + 1);
	}
	else
		comboBox->setCurrentIndex(-1);
}

void EditIndividualDialog::setupUiSetComboBoxForEvents (QComboBox *comboBox, Event *event)
{
	comboBox->setModel(_eventModelForComboBox);
	comboBox->setModelColumn(0);
	comboBox->adjustSize ();
	updateIndexForComboBox (comboBox, event);
	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setEdited()));
}

void EditIndividualDialog::setupUiEventLogPage ()
{
	setupUiSetComboBoxForEvents (ui->comboBoxBirthEvent, _individual->getBirthEvent ());
	setupUiSetComboBoxForEvents (ui->comboBoxDeathEvent, _individual->getDeathEvent ());

	ui->tableViewEvents->setSortingEnabled(true);
	ui->tableViewEvents->setModel(_eventModel);
	ui->tableViewEvents->resizeColumnsToContents();
	ui->tableViewEvents->setItemDelegateForColumn (EventModel::colDescription,
						       new RichTextEditorDelegate(_iconModel, _familyModel));
	connect(_eventModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		ui->tableViewEvents, SLOT(resizeColumnsToContents()));
	connect(_eventModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		this, SLOT(setEdited()));
	connect(_eventModel, SIGNAL(eventModelRegenerated()),
		this, SLOT(updateIndicesForComboBoxes()));
}

void EditIndividualDialog::setupUiUnionsPage ()
{
	int colCount = 0;
	QLabel *srnoHead = new QLabel ("Sr No.", this);
	ui->gridLayoutUnions->addWidget(srnoHead, 0, colCount++);
	QLabel *spouseHead = new QLabel("Spouse", this);
	ui->gridLayoutUnions->addWidget(spouseHead, 0, colCount++);
	QLabel *childrenHead = new QLabel("Children", this);
	ui->gridLayoutUnions->addWidget(childrenHead, 0, colCount++);
	QLabel *marriageEventHead = new QLabel("Marriage Event", this);
	ui->gridLayoutUnions->addWidget(marriageEventHead, 0, colCount++);
	QLabel *divorceEventHead = new QLabel("Divorce Event", this);
	ui->gridLayoutUnions->addWidget(divorceEventHead, 0, colCount++);

	for (int i = 0; i < _individual->spouseCount(); i++)
	{
		IDTYPE spid = _individual->getSpouse(i)->getId();
		colCount = 0;

		QLabel *srno = new QLabel (QString("%1").arg(i+1), this);
		ui->gridLayoutUnions->addWidget(srno, i+1, colCount++);

		QLabel *spouse = new QLabel(_individual->getSpouse(i)->getName(), this);
		spouse->setWordWrap(true);
		ui->gridLayoutUnions->addWidget(spouse, i+1, colCount++);

		QString childString = "";
		int offsprintCount = _individual->offspringCount(spid);
		if (offsprintCount)
		{
			childString += _individual->getOffspring(0, spid)->getUnderlinedName();
			if (offsprintCount > 1)
			{
				for (int j = 1; j < offsprintCount - 1; j++)
					childString += ", " + _individual->getOffspring(j, spid)->getUnderlinedName();
				childString += ", and " + _individual->getOffspring(offsprintCount - 1, spid)->getUnderlinedName();
			}
		}
		QLabel *children = new QLabel(childString, this);
		children->setWordWrap(true);
		ui->gridLayoutUnions->addWidget(children, i+1, colCount++);

		QComboBox *marriageEvent = new QComboBox(this);
		ui->gridLayoutUnions->addWidget(marriageEvent, i+1, colCount++);
		_marriageComboBoxes.append(marriageEvent);
		setupUiSetComboBoxForEvents (marriageEvent, _individual->getMarriageEvent (spid));

		QComboBox *divorceEvent = new QComboBox(this);
		ui->gridLayoutUnions->addWidget(divorceEvent, i+1, colCount++);
		_divorceComboBoxes.append(divorceEvent);
		setupUiSetComboBoxForEvents (divorceEvent, _individual->getDivorceEvent (spid));
	}

	connect(ui->toolButtonAddEvent, SIGNAL(clicked()), this, SLOT(addEvent()));
	connect(ui->toolButtonRemoveEvent, SIGNAL(clicked()), this, SLOT(delEvent()));

	_richTextEditor = new RichTextEditor(_iconModel, _familyModel, this);
	ui->verticalLayoutNotes->addWidget (_richTextEditor);
}

void EditIndividualDialog::setupUiNotesPage ()
{
	QString rawNotes = _individual->getNotes ();
	QString output;
	Utilities::convertFTMLToDisplayFormat (rawNotes, output, true, true, -1, _individual);
	_richTextEditor->setText (output);
	connect(_richTextEditor, SIGNAL(textChanged()), this, SLOT(setEdited()));
}

void EditIndividualDialog::setupUi()
{
	setWindowTitle(QString("Editing Individual: %1 (%2)").arg(_individual->getName()).arg(_individual->getId()));

	setupUiPersonalInformationPage ();
	setupUiEventLogPage ();
	setupUiUnionsPage ();
	setupUiNotesPage ();

	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
}

void EditIndividualDialog::setIndividualImage (QString imagePath) const
{
	ui->labelPhotoValue->setText (QString ("<a href=\"#\"><img width=\"%1\" src=\"%2\" /></a>")
				      .arg (SettingsManager::s_MaxEditDialogImageWidth).arg (imagePath));
}

bool EditIndividualDialog::save()
{
	if (isEdited())
	{
		QModelIndex index = _treeModel->index (_treeItem, Familier::e_col_name);
		_treeModel->setData (index, ui->lineEditName->text (), Qt::EditRole);
		index = _treeModel->index (_treeItem, Familier::e_col_dead);
		_treeModel->setData (index, ui->radioButtonDeadYes->isChecked (), Qt::EditRole);
		if (_individual->setImagePath(ui->lineEditImagePath->text()))
			emit imageEdited (_individual->getId ());
		_individual->setFacebookID (ui->lineEditFacebookID->text ());
		if (ui->checkBoxVillage->isChecked())
			_individual->setVillageInherited(true);
		else
		{
			_individual->setVillageInherited(false);
			_individual->setVillage(ui->lineEditVillage->text());
		}
		if (ui->checkBoxAddress->isChecked())
			_individual->setAddressInherited(true);
		else
		{
			_individual->setAddressInherited(false);
			_individual->setAddress(ui->lineEditAddress->text());
		}

		_individual->setAliases(_aliasesModel->getData());

		_individual->setBirthEvent(_eventModel->getEventFromIndex(
				ui->comboBoxBirthEvent->currentIndex() - 1));
		_individual->setDeathEvent(_eventModel->getEventFromIndex(
				ui->comboBoxDeathEvent->currentIndex() - 1));

		for (int i = 0; i < _individual->spouseCount(); i++)
		{
			Event *event;
			IDTYPE spid = _individual->getSpouse(i)->getId();
			_individual->setMarriageEvent(spid, _eventModel->getEventFromIndex(
					_marriageComboBoxes.at(i)->currentIndex() - 1));
			if ((event = _individual->getMarriageEvent(spid)))
				event->appendChild(Individual::s_IdToIndividualHash.value(spid));
			_individual->setDivorceEvent(spid, _eventModel->getEventFromIndex(
					_divorceComboBoxes.at(i)->currentIndex() - 1));
			if ((event = _individual->getDivorceEvent(spid)))
				event->appendChild(Individual::s_IdToIndividualHash.value(spid));
		}

		_individual->setNotes (_richTextEditor->toHtml ());
	}

	return isEdited();
}

void EditIndividualDialog::setEdited()
{
	EditDialog::setEdited();
	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
}

void EditIndividualDialog::loadImage()
{
	QString filePath = QFileDialog::getOpenFileName
			   (this, tr ("Select file to Open"), SettingsManager::s_ProfileImageDirectory,
			    tr("Image Files (*.png *.jpg *.gif) ;; All Files (*.*)"));
	if (filePath.isEmpty())
		return;

	QDir documentDirectory (SettingsManager::s_DocumentDirectoryPath);
	QString relativeFilePath = documentDirectory.relativeFilePath (filePath);
	SettingsManager::s_ProfileImageDirectory = filePath;
	SettingsManager::s_ProfileImageDirectory.replace(QRegExp("/[^/]*$"), "");

	ui->lineEditImagePath->setText(QDir::toNativeSeparators(relativeFilePath));

	setIndividualImage (filePath);
}

void EditIndividualDialog::setVillageInherited(bool inherited)
{
	ui->lineEditVillage->setEnabled(!inherited);
	ui->checkBoxVillage->setChecked(inherited);
}

void EditIndividualDialog::setAddressInherited(bool inherited)
{
	ui->lineEditAddress->setEnabled(!inherited);
	ui->checkBoxAddress->setChecked(inherited);
}

void EditIndividualDialog::addEvent()
{
	int row = _eventModel->rowCount(QModelIndex());
	if (!_eventModel->insertRow(row, QModelIndex()))
	{
		QMessageBox::warning(this, "Error", "Adding of new event failed.");
		return;
	}

	QModelIndex newIndex = _eventModel->index(row, 0, QModelIndex());
	_eventModel->setData(_eventModel->index(newIndex.row(), _eventModel->colIsMember, newIndex.parent()),
			true, Qt::EditRole);
	ui->tableViewEvents->scrollTo(newIndex);
	ui->tableViewEvents->edit(newIndex);
	setEdited();
}

void EditIndividualDialog::delEvent()
{
	QModelIndex currentIndex = ui->tableViewEvents->currentIndex();
	if (!currentIndex.isValid())
	{
		QMessageBox::warning(this, "Error", "No event selected");
		return;
	}

	if (QMessageBox::question(this, "Delete event",
				  "Are you sure you want to delete selected event?",
				  QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		if (_eventModel->removeRow(currentIndex.row(), QModelIndex()))
			setEdited();
	}
}

void EditIndividualDialog::updateIndicesForComboBoxes ()
{
	updateIndexForComboBox (ui->comboBoxBirthEvent, _individual->getBirthEvent ());
	updateIndexForComboBox (ui->comboBoxDeathEvent, _individual->getDeathEvent ());

	for (int i = 0; i < _marriageComboBoxes.size (); i++)
		updateIndexForComboBox (_marriageComboBoxes.at (i), _individual->getMarriageEventIndex (i));

	for (int i = 0; i < _marriageComboBoxes.size (); i++)
		updateIndexForComboBox (_divorceComboBoxes.at (i), _individual->getDivorceEventIndex (i));
}
