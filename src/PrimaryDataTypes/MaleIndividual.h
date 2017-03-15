#ifndef MALEINDIVIDUAL_H
#define MALEINDIVIDUAL_H

#include "Individual.h"

class MaleIndividual : public Individual
{
public:
	static int	s_MaleIndividualCount;

public:
	MaleIndividual (const IDTYPE id,
			const QDateTime &creationDateTime = QDateTime::currentDateTime(),
			const QDateTime &modificationDateTime = QDateTime::currentDateTime(),
			const unsigned long modificationCount = 0);
	MaleIndividual (const QDateTime &creationDateTime = QDateTime::currentDateTime(),
			const QDateTime &modificationDateTime = QDateTime::currentDateTime(),
			const unsigned long modificationCount = 0);
	MaleIndividual (const QDomElement &domElement);
	MaleIndividual (const MaleIndividual &maleIndividual);
	virtual ~MaleIndividual ();
	virtual void setupItemLinks (const QDomElement &domElement);
	virtual void setupModel(TreeModel *model, int row,
				QModelIndex &parent, QProgressDialog &progress);
	virtual Individual* adoptAndReturnNewIndividual ();

	virtual bool			appendChild		(const IdableWithChildren *child);
	virtual bool			removeChild		(const IdableWithChildren *child);
	virtual bool			removeChild		(const IDTYPE chid);
	virtual bool			insertChild		(const long chidx, const IdableWithChildren *child);
	virtual bool			removeChildAt		(const long chidx);
	virtual IdableWithChildren*	takeChildAt		(const long chidx);

	virtual bool		isMale			() const { return true; }
	virtual bool		isFemale		() const { if (_adoptedTo) return _adoptedTo-> isFemale(); return false; }

	friend QTextStream &	operator << 		(QTextStream &stream, MaleIndividual &maleIndividual);

	friend QDebug		operator <<		(QDebug debug, MaleIndividual &maleIndividual);

	virtual QString		genderBias		(const EnglishWord ew, const bool capitalise) const;

	virtual void		updateStatistics	();
	virtual void		unsetStatsUpToDate	();

	virtual bool		fullTextSearch		(const QRegExp &searchRegExp,
							 QStringList &resultsList) const;

	virtual QList <Individual*>	getSalaSalhajs		(bool &allok) const;
	virtual QList <Individual*>	getSaadhuSalis		(bool &allok) const;

	virtual QList <Individual*>	getJethJethanis		(bool &) const { return QList <Individual*>(); }
	virtual QList <Individual*>	getDevarDevarani	(bool &) const { return QList <Individual*>(); }
	virtual QList <Individual*>	getNanadNandoi		(bool &) const { return QList <Individual*>(); }
};

#endif /* MALEINDIVIDUAL_H */

