#include "MaleIndividual.h"
#include "TreeModel.h"

#define GENDERBIASCASE(malew,femalew) \
	case malew ## femalew: \
	{ \
		QString temp (#malew); \
		if (capitalise) \
			Idable::toTitleCase (temp); \
		return temp; \
	}

int MaleIndividual::s_MaleIndividualCount = 0;

MaleIndividual::MaleIndividual (const IDTYPE id,
				const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
				const unsigned long modificationCount)
				: Individual (id, creationDateTime, modificationDateTime, modificationCount)
{
	s_MaleIndividualCount ++;
}

MaleIndividual::MaleIndividual (const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
				const unsigned long modificationCount)
					: Individual (creationDateTime, modificationDateTime, modificationCount)
{
	s_MaleIndividualCount ++;
}

MaleIndividual::MaleIndividual (const QDomElement &domElement) : Individual (domElement)
{
	s_MaleIndividualCount ++;
}

MaleIndividual::MaleIndividual (const MaleIndividual &maleIndividual) : Individual (maleIndividual)
{

}

MaleIndividual::~MaleIndividual ()
{
	if (!_copyConstructed)
		s_MaleIndividualCount --;
}

void MaleIndividual::setupItemLinks (const QDomElement &domElement)
{
	blockRecordingOfItemModifications();

	Individual::setupItemLinks (domElement);

	foreach (IdableWithChildren *idableWithChildren, _children)
	{
		Union *aUnion = dynamic_cast<Union *>(idableWithChildren);
		for (int i = 0; i < aUnion->childCount (); i++)
		{
			Individual *child = dynamic_cast<Individual *>(aUnion->childAt (i));
			child->blockRecordingOfItemModifications ();
			child->setParentUnion (aUnion);
			child->setFamily (_family);
			child->unblockRecordingOfItemModifications ();
		}
	}

	unblockRecordingOfItemModifications();
}

void MaleIndividual::setupModel(TreeModel *model, int row,
				QModelIndex &parent, QProgressDialog &progress)
{
	blockRecordingOfItemModifications();

	_modelSetupInProgress = true;

	TreeItem *parentItem = model->getItem(parent);
	model->insertRow(const_cast<MaleIndividual *>(this), true, row, parent, parentItem->isSpouseItem());

	QModelIndex index = model->index(row, 0, parent);
	for (int crow = 0; crow < _children.count (); crow ++)
	{
		Union *aUnion = dynamic_cast<Union *>(_children.value (crow));
		aUnion->getWife ()->setupModel (model, crow, index, progress);
	}

	_modelSetupInProgress = false;

	unblockRecordingOfItemModifications();

	Familier::setupModel(model, row, parent, progress);
}

Individual *MaleIndividual::adoptAndReturnNewIndividual ()
{
	MaleIndividual *individual = new MaleIndividual;
	updateAdoptedIndividual (individual);
	return individual;
}

bool MaleIndividual::appendChild (const IdableWithChildren *child)
{
	if (IdableWithChildren::appendChild(child))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

bool MaleIndividual::removeChild (const IdableWithChildren *child)
{
	if (IdableWithChildren::removeChild(child))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

bool MaleIndividual::removeChild (const IDTYPE chid)
{
	if (IdableWithChildren::removeChild(chid))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

bool MaleIndividual::insertChild (const long chidx, const IdableWithChildren *child)
{
	if (IdableWithChildren::insertChild(chidx, child))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

bool MaleIndividual::removeChildAt (const long chidx)
{
	if (IdableWithChildren::removeChildAt(chidx))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

IdableWithChildren* MaleIndividual::takeChildAt (const long chidx)
{
	unsetStatsUpToDate();
	return IdableWithChildren::takeChildAt(chidx);
}

QTextStream& operator << (QTextStream &stream, MaleIndividual &maleIndividual)
{
	stream << dynamic_cast <Individual &> (maleIndividual);
	return stream;
}

QDebug operator << (QDebug debug, MaleIndividual &maleIndividual)
{
	debug << dynamic_cast <Individual &> (maleIndividual);
	return debug;
}

QString MaleIndividual::genderBias(const EnglishWord ew, const bool capitalise) const
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

void MaleIndividual::updateStatistics ()
{
	_statsUpToDate = true;
	_sonCount = 0;
	_daughterCount = 0;
	_marriedDaughterCount = 0;
	_spouseCount = 0;
	_deadCount = 0;
	_generations = 0;

	for (int i = 0; i < _children.count(); i++)
	{
		Union *aunion = dynamic_cast<Union *> (_children[i]);
		if (aunion)
		{
			Individual *wife = aunion->getWife();
			if (wife->isNonDummy ())
			{
				_spouseCount ++;
				if (wife->isDead())
					_deadCount ++;
			}

			for (int j = 0; j < aunion->childCount(); j++)
			{
				Individual *child = dynamic_cast<Individual *> (aunion->childAt(j));
				if (child)
				{
					unsigned long sonCount;
					unsigned long daughterCount;
					unsigned long marriedDaughterCount;
					unsigned long spouseCount;
					unsigned long deadCount;
					unsigned long generations;
					child->getStatistics (sonCount, daughterCount, marriedDaughterCount,
							spouseCount, deadCount, generations);
					_sonCount += sonCount;
					_daughterCount += daughterCount;
					_marriedDaughterCount += marriedDaughterCount;
					_spouseCount += spouseCount;
					if (!child->isFemale() || !child->hasSpouse())
						_deadCount += deadCount;
					if (_generations < generations)
						_generations = generations;
				}
			}
		}
	}

	_sonCount ++;
	if (_dead)
		_deadCount ++;
	_generations ++;

	_totalCount = _sonCount + _daughterCount - _marriedDaughterCount + _spouseCount;
}

void MaleIndividual::unsetStatsUpToDate()
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

	if (Individual *father = getFather ())
	{
		father->unsetStatsUpToDate();
		if (Individual *mother = getMother ())
			mother->unsetStatsUpToDate();
	}
	else if (_family)
		_family->unsetStatsUpToDate();
}

bool MaleIndividual::fullTextSearch (const QRegExp &searchRegExp, QStringList &resultsList) const
{
	bool found = Individual::fullTextSearch (searchRegExp, resultsList);

	if (searchRegExp.pattern ().toLower () == "male")
	{
		found = true;
		resultsList << "Sex: Male";
	}

	return found;
}

QList <Individual*> MaleIndividual::getSalaSalhajs (bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *spouse, getSpouses ())
		foreach (Individual *sala, spouse->getBrothers () + spouse->getMaleCousins (allok))
			addIndividualAndSpouse (sala, retList);
	return retList;
}

QList <Individual*> MaleIndividual::getSaadhuSalis (bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *spouse, getSpouses ())
		foreach (Individual *sali, spouse->getSisters () + spouse->getFemaleCousins (allok))
			addIndividualAndSpouse (sali, retList);
	return retList;
}
