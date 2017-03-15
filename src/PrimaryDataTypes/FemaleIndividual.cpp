#include "FemaleIndividual.h"
#include "Family.h"
#include "TreeModel.h"

#define GENDERBIASCASE(malew,femalew) \
	case malew ## femalew: \
		{ \
			QString temp (#femalew); \
			if (capitalise) \
				Idable::toTitleCase (temp); \
			return temp; \
		}

int FemaleIndividual::s_FemaleIndividualCount = 0;

FemaleIndividual::FemaleIndividual(const unsigned long id,
				   const QDateTime &creationDateTime,
				   const QDateTime &modificationDateTime,
				   const unsigned long modificationCount)
					   : Individual (id, creationDateTime, modificationDateTime, modificationCount)
{
	s_FemaleIndividualCount ++;
}

FemaleIndividual::FemaleIndividual (const QDateTime &creationDateTime,
				    const QDateTime &modificationDateTime,
				    const unsigned long modificationCount)
					    : Individual (creationDateTime, modificationDateTime, modificationCount)
{
	s_FemaleIndividualCount ++;
}

FemaleIndividual::FemaleIndividual (const QDomElement &domElement) : Individual (domElement)
{
	s_FemaleIndividualCount ++;
}

FemaleIndividual::FemaleIndividual (const FemaleIndividual &femaleIndividual)
	: Individual (femaleIndividual)
{

}

FemaleIndividual::~FemaleIndividual ()
{
	if (!_copyConstructed)
		s_FemaleIndividualCount --;
}

QVariant FemaleIndividual::data (const int col, const int role, const IDTYPE spouseID)
{
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch (col)
		{
		case e_col_sonCount:
		case e_col_daughterCount:
		case e_col_marriedDaughterCount:
		case e_col_spouseCount:
		case e_col_aliveCount:
		case e_col_deadCount:
		case e_col_totalCount:
		case e_col_generations:
			return QVariant ();
		default:
			break;
		}
	}

	return Individual::data (col, role, spouseID);
}

void FemaleIndividual::setupItemLinks (const QDomElement &domElement)
{
	blockRecordingOfItemModifications();
	Individual::setupItemLinks (domElement);
	unblockRecordingOfItemModifications();
}

void FemaleIndividual::setupModel(TreeModel *model, int row,
				  QModelIndex &parent, QProgressDialog &progress)
{
	blockRecordingOfItemModifications();

	_modelSetupInProgress = true;

	TreeItem *parentItem = model->getItem(parent);
	/* If parent item is male then this item is in the role of
	   spouse. And only in this case we need to process child
	   items. In the role of daughter there are no child items.
	 */
	if (parentItem->isIndividual())
	{
		if (!parentItem->isFemale())
		{
			IDTYPE spid = parentItem->id();
			for (int i = 0; i < _children.count(); i ++)
			{
				Union *aUnion = dynamic_cast<Union *>(_children.value (i));
				if (aUnion->getHusband()->getId() != spid)
					continue;

				model->insertRow(const_cast<FemaleIndividual *>(this), true,
						 row, parent, aUnion->getId ());
				QModelIndex index = model->index(row, 0, parent);
				for (int crow = 0; crow < aUnion->childCount(); crow++)
				{
					Individual *individual = dynamic_cast<Individual *>(aUnion->childAt(crow));
					individual->blockRecordingOfItemModifications ();
					individual->setFamily (aUnion->getHusband ()->getFamily ());
					individual->unblockRecordingOfItemModifications ();
					individual->setupModel(model, crow, index, progress);
				}
			}
		}
		else
			model->insertRow(const_cast<FemaleIndividual *>(this), true,
					 row, parent, parentItem->isFemale());
	}
	else
		model->insertRow(const_cast<FemaleIndividual *>(this), true, row, parent);

	_modelSetupInProgress = false;

	unblockRecordingOfItemModifications();

	Familier::setupModel(model, row, parent, progress);
}

Individual *FemaleIndividual::adoptAndReturnNewIndividual ()
{
	FemaleIndividual *individual = new FemaleIndividual;
	updateAdoptedIndividual (individual);
	return individual;
}

QTextStream& operator << (QTextStream &stream, FemaleIndividual &femaleIndividual)
{
	stream << dynamic_cast <Individual &> (femaleIndividual);
	return stream;
}

QDebug operator << (QDebug debug, FemaleIndividual &femaleIndividual)
{
	debug << dynamic_cast <Individual &> (femaleIndividual);
	return debug;
}

QString FemaleIndividual::getSurName () const
{
	Individual *latestSpouse = getSpouse (spouseCount()-1);
	if (latestSpouse && latestSpouse->getFamily ())
		return latestSpouse->getFamily()->getName();

	return getMaidenName();
}

QString FemaleIndividual::getFullName () const
{
	return QString("%1 %2").arg(_name).arg(getSurName());
}

QString FemaleIndividual::getMaidenName () const
{
	if (_family)
		return _family->getName();

	return "";
}

QString FemaleIndividual::genderBias(const EnglishWord ew, const bool capitalise) const
{
	switch (ew)
	{
		GENDERBIASCASE(he, she)
		GENDERBIASCASE(him, her)
		GENDERBIASCASE(his, her)
		GENDERBIASCASE(brother, sister)
		GENDERBIASCASE(husband, wife)
		GENDERBIASCASE(father, mother)
	default:
		break;
	}

	return "";
}

void FemaleIndividual::updateStatistics ()
{
	_statsUpToDate = true;
	_sonCount = 0;
	_daughterCount = 1;
	if (hasSpouse())
		_marriedDaughterCount = 1;
	else
		_marriedDaughterCount = 0;
	_spouseCount = 0;

	if (_dead)
		_deadCount = 1;
	else
		_deadCount = 0;
	_generations = 1;

	_totalCount = 1;
}

void FemaleIndividual::unsetStatsUpToDate()
{
	/* If _statsUpToDate is already false, we do not need to do
	   anything. Even its parent items will be false anyway.
	 */
	if (!_statsUpToDate)
		return;

	/* This step has to be done even if _modelSetupInProgress is
	   true. Only upwards propagation has to be stopped, until
	   the model setup completes.
	 */
	_statsUpToDate = false;

	if (_modelSetupInProgress)
		return;

	for (int i = 0; i < spouseCount(); i++)
	{
		Individual *husband = getSpouse(i);
			husband->unsetStatsUpToDate();
	}
}

bool FemaleIndividual::fullTextSearch (const QRegExp &searchRegExp, QStringList &resultsList) const
{
	bool found = Individual::fullTextSearch (searchRegExp, resultsList);

	if (searchRegExp.pattern ().toLower () == "female")
	{
		found = true;
		resultsList << "Sex: Female";
	}

	return found;
}

QList <Individual*> FemaleIndividual::getJethJethanis (bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *spouse, getSpouses ())
		foreach (Individual *jeth, spouse->getElderBrothers ()
			 + spouse->getElderMaleCousins (allok))
			addIndividualAndSpouse (jeth, retList);
	return retList;
}

QList <Individual*> FemaleIndividual::getDevarDevarani (bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *spouse, getSpouses ())
		foreach (Individual *devar, spouse->getYoungerBrothers ()
			 + spouse->getYoungerMaleCousins (allok))
			addIndividualAndSpouse (devar, retList);
	return retList;
}

QList <Individual*> FemaleIndividual::getNanadNandoi (bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *spouse, getSpouses ())
		foreach (Individual *nanad, spouse->getSisters ()
			 + spouse->getFemaleCousins (allok))
			addIndividualAndSpouse (nanad, retList);
	return retList;
}
