#ifndef FAMILIER_H
#define FAMILIER_H

#include <QStringList>
#include "IdableWithChildren.h"
#include <QModelIndex>
#include <QProgressDialog>

class TreeItem;
class TreeModel;

class Familier : public IdableWithChildren
{
public:
	enum EnglishWord
	{
		heshe, himher, hisher, brothersister, husbandwife, fathermother,
		/* for deathBias */
		hashad, iswas,
		/* for pluralBias*/
		isare, waswere, spousespouses, membermembers,
		generationgenerations
	};

private:
	long			_refCount;
	TreeItem *		_treeItem;

public:
	enum t_columns {
		e_col_name,
		e_col_link,			// Individual
		e_col_alias,
		e_col_sex,			// Individual
		e_col_dead,			// Individual
		e_col_birthDate,		// Individual
		e_col_marriageDate,		// Individual
		e_col_deathDate,		// Individual
		e_col_facebookID,		// Individual
		e_col_dummyOrBase,		// Individual & Family
		e_col_myGeneration,		// Individual
		e_col_sonCount,
		e_col_daughterCount,
		e_col_marriedDaughterCount,
		e_col_spouseCount,
		e_col_aliveCount,
		e_col_deadCount,
		e_col_totalCount,
		e_col_generations,
		e_col_maximum			// None

	} _columns;

protected:
	//Internal data
	QStringList		_aliases;

	/* _modelSetupInProgress is to be used in unsetStatsUpToDate().
	   If it is set in MaleIndividual and FemaleIndividual classes,
	   unsetStatsUpToDate() will not do anything. The setupModel()
	   function in these classes will set this variable to true
	   upon execution start and to false at the end, followed by a
	   call to unsetStatsUpToDate() function. This is for efficiency
	   reasons.
	 */
	bool			_modelSetupInProgress;
	bool			_statsUpToDate;
	unsigned int		_sonCount;
	unsigned int		_daughterCount;
	unsigned int		_marriedDaughterCount;
	unsigned int		_spouseCount;
	unsigned int		_deadCount;
	unsigned int		_generations;
	unsigned int		_totalCount;

public:

	Familier (const IDTYPE id,
		  const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
		  const unsigned long modificationCount);
	Familier (const QDomElement &domElement);
	Familier (const Familier &familier);
	virtual ~Familier ();
	virtual void setupItemLinks (const QDomElement &domElement);
	//virtual void isValid () const;
	virtual void setupModel (TreeModel *model, int row,
				 QModelIndex &parent, QProgressDialog &progress);

	GETFUNCTION	(RefCount,	long,		refCount)
	GETFUNCTION
	(TreeItem,	TreeItem *,	treeItem)
	GETFUNCTION	(Aliases,	QStringList,	aliases)

	void			refCountPP			() { _refCount++; }
	void			refCountMM			() { _refCount--; }

	SETFUNCTIONDECL (TreeItem,	TreeItem *,	treeItem)
	SETFUNCTIONDECL	(Aliases,	QStringList,	aliases)

	static int		columnCount			();
	static QVariant		headerData			(int section, Qt::Orientation orientation, int role = Qt::DisplayRole);
	virtual Qt::ItemFlags	flags				(const int col, const bool isSpouseItem) const;
	virtual QVariant	data				(const int col, const int role, const IDTYPE spouseID);
	virtual bool		setData				(const int col, const QVariant &value, const IDTYPE spouseID);

	friend QTextStream &	operator <<			(QTextStream &stream, Familier &familier);

	friend QDebug		operator <<			(QDebug debug, Familier &familier);

	bool			operator ==			(const Familier &) const;
	bool			operator !=			(const Familier &) const;

	virtual QString		pluralBias			(const EnglishWord ew,
								 const int count,
								 const bool capitalise) const;

	QString			getAliasesAsCommaSeparatedList	(bool useAndForLastString) const;
	bool			setAliasesFromCommaSeparatedList(QString aliases);

	virtual void		updateStatistics		() = 0;
	virtual QString		getAnchor			(const IDTYPE spouseIdForCurrentEntryOfFemale,
								 const bool useFullName,
								 const bool createAnchor) = 0;
	
	void			getStatistics			(unsigned long &sonCount, unsigned long &daughterCount,
								 unsigned long &marriedDaughterCount, unsigned long &spouseCount,
								 unsigned long &deadCount, unsigned long &generations);
	int			getGenerations			() const { return _generations; }
	QString			getStatisticsHTML		() const;
	virtual QString		getToolTip			(const bool createAnchor, const IDTYPE spouseIdForCurrentEntryOfFemale) = 0;
	bool			areStatsUpToDate		() const { return _statsUpToDate; }
	virtual void		unsetStatsUpToDate		() { _statsUpToDate = false; }
	virtual QString		getItemViewPage			(const IDTYPE spouseIdForCurrentEntryOfFemale);
	virtual bool		fullTextSearch			(const QRegExp &searchRegExp,
								 QStringList &resultsList) const;
};

QTextStream & operator << (QTextStream &stream, Familier &oFamilier);

#endif /* FAMILIER_H */

