#ifndef FEMALEINDIVIDUAL_H
#define FEMALEINDIVIDUAL_H

#include "Individual.h"

class FemaleIndividual : public Individual
{
public:
	static int	s_FemaleIndividualCount;

public:
	FemaleIndividual (const IDTYPE id,
			  const QDateTime &creationDateTime = QDateTime::currentDateTime(),
			  const QDateTime &modificationDateTime = QDateTime::currentDateTime(),
			  const unsigned long modificationCount = 0);
	FemaleIndividual (const QDateTime &creationDateTime = QDateTime::currentDateTime(),
			  const QDateTime &modificationDateTime = QDateTime::currentDateTime(),
			  const unsigned long modificationCount = 0);
	FemaleIndividual (const QDomElement &domElement);
	FemaleIndividual (const FemaleIndividual &femaleIndividual);
	virtual ~FemaleIndividual ();

	virtual QVariant	data			(const int col, const int role,
							 const IDTYPE spouseID);

	virtual void setupItemLinks (const QDomElement &domElement);
	virtual void setupModel(TreeModel *model, int row,
				QModelIndex &parent, QProgressDialog &progress);
	virtual Individual* adoptAndReturnNewIndividual ();

	virtual bool		isMale			() const { return false; }
	virtual bool		isFemale		() const { return true; }

	friend QTextStream &	operator << 		(QTextStream &stream, FemaleIndividual &femaleIndividual);

	friend QDebug		operator <<		(QDebug debug, FemaleIndividual &femaleIndividual);

	virtual QString		getSurName		() const;
	virtual QString		getFullName		() const;

	QString			getMaidenName		() const;

	virtual QString		genderBias		(const EnglishWord ew, const bool capitalise) const;

	virtual void		updateStatistics	();
	virtual void		unsetStatsUpToDate	();

	virtual bool		fullTextSearch		(const QRegExp &searchRegExp,
							 QStringList &resultsList) const;

	virtual QList <Individual*>	getSalaSalhajs		(bool &) const { return QList <Individual*> (); }
	virtual QList <Individual*>	getSaadhuSalis		(bool &) const { return QList <Individual*> (); }

	virtual QList <Individual*>	getJethJethanis		(bool &allok) const;
	virtual QList <Individual*>	getDevarDevarani	(bool &allok) const;
	virtual QList <Individual*>	getNanadNandoi		(bool &allok) const;
};

#endif /* FEMALEINDIVIDUAL_H */

