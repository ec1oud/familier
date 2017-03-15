#ifndef FAMILY_H
#define FAMILY_H

#include "Familier.h"

/*
 * The children (_children) QList is a list
 * of primary members for the Family.
 */

class Family : public Familier
{
	DECLAREAUTOITEMMANAGEMENT(Family)

public:
	static QMap<QString, Family *>		s_NameToBaseFamilyMap;
	static QMap<QString, Family *>		s_NameToNonBaseFamilyMap;

	static bool	familyExists		(const QString &familyName);
	static Family *	getFamilyWithName	(const QString &familyName);

	enum AnchorType { primary_members };

protected:
	bool			_base;
	
public:
	Family (const IDTYPE id,
		QDateTime creationDateTime = QDateTime::currentDateTime(),
		QDateTime modificationDateTime = QDateTime::currentDateTime(),
		const unsigned long modificationCount = 0);
	Family (QDateTime creationDateTime = QDateTime::currentDateTime(),
		QDateTime modificationDateTime = QDateTime::currentDateTime(),
		const unsigned long modificationCount = 0);
	Family (const QDomElement &domElement);
	Family (const Family &family);
	virtual ~Family ();
	virtual void setupItemLinks (const QDomElement &domElement);
	//virtual bool isValid () const;
	virtual IdableWithChildren* getItemWithId (const IDTYPE id) const;
	virtual void setupModel(TreeModel *model, int row,
				QModelIndex &parent, QProgressDialog &progress);

private:
	void init ();
	void initLinks ();
	void initAsPerBase ();

public:
	ISFUNCTION		(Base,		base)

	GETSETMAXIDHEAD (Family)

	SETFUNCTIONDECL			(Base,		bool,		base)
	virtual SETFUNCTIONDECL		(Name,		QString &,	name)

	virtual Qt::ItemFlags		flags			(const int col, const bool isSpouseItem) const;
	virtual QVariant		data			(const int col, const int role, const IDTYPE spouseID);
	virtual bool			setData			(const int col, const QVariant &value, const IDTYPE spouseID);

	virtual QString			getChildTypeName	() const;
	virtual bool			appendChild		(const IdableWithChildren *child);
	virtual bool			removeChild		(const IdableWithChildren *child);
	virtual bool			removeChild		(const IDTYPE chid);
	virtual bool			insertChild		(const long chidx, const IdableWithChildren *child);
	virtual bool			removeChildAt		(const long chidx);
	virtual IdableWithChildren*	takeChildAt		(const long chidx);

	friend QTextStream &	operator <<			(QTextStream &stream, Family &family);

	friend QDebug		operator <<			(QDebug debug, Family &family);

	virtual void		updateStatistics		();
	virtual QString		getAnchor			(const IDTYPE spouseIdForCurrentEntryOfFemale,
								 const bool useFullName,
								 const bool createAnchor);
	virtual QString		getListOfAnchors		(const int type,
								 const bool useFullName,
								 const bool createAnchor);

	virtual QString		getToolTip			(const bool createAnchor,
								 const IDTYPE spouseIdForCurrentEntryOfFemale);
	virtual QString		getItemViewPage			(const IDTYPE spouseIdForCurrentEntryOfFemale);

	virtual bool		fullTextSearch			(const QRegExp &searchRegExp,
								 QStringList &resultsList) const;
};

#endif /* FAMILY_H */

