#ifndef UNION_H
#define UNION_H

#include "IdableWithChildren.h"
#include "Event.h"

/*
 * The children (_children) QList is the list of
 * children produced from the union.
 */

class Event;
class Individual;

class Union : public IdableWithChildren
{
	DECLAREAUTOITEMMANAGEMENT(Union)

protected:
	Individual *		_husband;
	Individual *		_wife;
	Event *			_marriageEvent;
	Event *			_divorceEvent;
	//QList<IDTYPE>		_adoptedChildren;

public:
	Union (const QDateTime &creationDateTime = QDateTime::currentDateTime(),
	       const QDateTime &modificationDateTime = QDateTime::currentDateTime(),
	       const unsigned long modificationCount = 0);
	Union (const QDomElement &domElement);
	Union (const Union &aUnion);
	virtual ~Union ();
	virtual void setupItemLinks (const QDomElement &domElement);
	virtual IdableWithChildren* getItemWithId (const IDTYPE id) const;

private:
	void init ();
	void initLinks ();

public:
	virtual bool			appendChild		(const IdableWithChildren *child);
	virtual bool			removeChild		(const IdableWithChildren *child);
	virtual bool			removeChild		(const IDTYPE chid);
	virtual bool			insertChild		(const long chidx, const IdableWithChildren *child);
	virtual bool			removeChildAt		(const long chidx);
	virtual IdableWithChildren*	takeChildAt		(const long chidx);
	void				propagateChildAlteration() const;

	virtual QString		getName			() const;

	GETFUNCTION	(Husband,		Individual *,	husband)
	GETFUNCTION	(Wife,			Individual *,	wife)
	GETFUNCTION	(MarriageEvent,		Event *,	marriageEvent)
	GETFUNCTION	(DivorceEvent,		Event *,	divorceEvent)

	GETSETMAXIDHEAD (Union)
	
	SETFUNCTIONDECL	(Husband,		Individual *,	husbandId)
	SETFUNCTIONDECL	(Wife,			Individual *,	wifeId)
	SETFUNCTIONDECL (MarriageEvent,		Event *,	marriageEvent)
	SETFUNCTIONDECL (DivorceEvent,		Event *,	divorceEvent)

	//bool			makeChildAdopted	(const IDTYPE chid);
	//bool			makeChildUnadopted	(const IDTYPE chid);
	//bool			isChildAdopted		(const IDTYPE chid);

	virtual QString		getChildTypeName	() const;

	friend QTextStream &	operator <<		(QTextStream &stream, Union &aUnion);
	QTextStream &		toGEDCOM		(QTextStream &stream) const;
	void			fromGEDCOM		(const QStringList &gedcomEntryStringList,
							 const QHash<QString, Individual*> &xRefToIndividualHash,
							 const QHash <QString, QStringList> &xRefToNoteGEDCOMInfoHash);

	friend QDebug		operator <<		(QDebug debug, Union &aUnion);

	Individual*		getSpouse		(bool spouseGenderIsFemale) const;
};

#endif /* UNION_H */
