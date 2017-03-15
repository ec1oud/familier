#ifndef IDABLEWITHCHILDREN_H
#define IDABLEWITHCHILDREN_H

#include "Idable.h"

#include <QList>

#define DECLAREAUTOITEMMANAGEMENT(type) \
public: \
	static IDTYPE			s_Max ## type ## Id; \
	static QList<type *>		s_ ## type ## List; \
	static QHash <IDTYPE, type *>	s_IdTo ## type ## Hash;

#define DEFINEAUTOITEMMANAGEMENT(type) \
IDTYPE type::s_Max ## type ## Id = 0; \
QList<type *> type::s_ ## type ## List; \
QHash <IDTYPE, type *> type::s_IdTo ## type ## Hash;

#define INSERTITEMINTOITEMMANAGEMENT(type) \
if (_id > 0) \
{ \
	if (_id > s_Max ## type ## Id) \
		s_Max ## type ## Id = _id; \
	s_ ## type ## List.append(this); \
	s_IdTo ## type ## Hash[_id] = this; \
}

#define REMOVEITEMFROMITEMMANAGEMENT(type) \
	if (!_copyConstructed) \
	{ \
		s_ ## type ## List.removeOne(this); \
		s_IdTo ## type ## Hash.remove(_id); \
		if (_id == s_Max ## type ## Id) \
			s_Max ## type ## Id --; \
	}

#define DEFINITIONOFGETITEMWITHID(classtype,searchtype) \
IdableWithChildren* classtype::getItemWithId (const IDTYPE id) const \
{ \
	if (searchtype::s_IdTo ## searchtype ## Hash.contains(id)) \
		return searchtype::s_IdTo ## searchtype ## Hash.value(id); \
	else \
		return NULL; \
}

#define LOADITEMFROMXMLWITHOUTCREATION(type,variable) \
	IDTYPE variable = domElement.attribute( # variable, "0").trimmed().toULong(); \
	if (!variable || !type::s_IdTo ## type ## Hash.contains(variable)) \
		_ ## variable = NULL; \
	else \
		_ ## variable = dynamic_cast<type *>(type::s_IdTo ## type ## Hash.value(variable));

/* Adds automatic item management. */
#define GETSETMAXIDHEAD(class) \
public: \
	static IDTYPE getMax ## class ## Id (); \
	virtual SETFUNCTIONDECL (Id, IDTYPE, id)

#define GETSETMAXIDCPP(class) \
IDTYPE class::getMax ## class ## Id () \
{ \
	return s_Max ## class ## Id; \
} \
\
bool class::setId (const IDTYPE id) \
{ \
	if (id > s_Max ## class ## Id) \
		s_Max ## class ## Id = id; \
\
	REMOVEITEMFROMITEMMANAGEMENT(class) \
	bool what = Idable::setId (id); \
	INSERTITEMINTOITEMMANAGEMENT(class) \
	return what; \
}

class IdableWithChildren : public Idable
{
//	QString				_displayTypeOfChildren;

public:
	static int			s_commonNotesID;

protected:
	bool				_copyConstructed;
	QList<IdableWithChildren*>	_children;
	
public:
	IdableWithChildren (const IDTYPE id,
			    const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
			    const unsigned long modificationCount);
	IdableWithChildren (const QDomElement &domElement);
	IdableWithChildren (const IdableWithChildren &idableWithChildren);
	virtual ~IdableWithChildren ();
	virtual void setupItemLinks (const QDomElement &domElement);
	//virtual bool isValid () const;
	virtual IdableWithChildren *	getItemWithId		(const IDTYPE id) const = 0;

public:

	virtual QString			getChildTypeName	() const = 0;
	virtual bool			appendChild		(const IdableWithChildren *child);
	virtual bool			removeChild		(const IdableWithChildren *child);
	virtual bool			removeChild		(const IDTYPE chid);
	virtual bool			insertChild		(const long chidx, const IdableWithChildren *child);
	virtual bool			removeChildAt		(const long chidx);
	virtual IdableWithChildren*	childAt			(const long chidx) const;
	virtual IdableWithChildren*	takeChildAt		(const long chidx);
	virtual bool			swapChildren		(const long chidx1, const long chidx2);
	virtual bool			moveChildUp		(const long chidx);
	virtual bool			moveChildDown		(const long chidx);
	QList<IdableWithChildren *>	getChildren		() const;
	void				clearChildren		();

	long				childCount		() const { return _children.count (); }
	long				childNumber		(const IdableWithChildren *child) const;
	long				indexOfChildFromId	(const IDTYPE chid) const;
	IDTYPE				idOfChildFromIndex	(const long chidx) const;
	bool				isChild			(const IDTYPE chid) const;
	bool				hasChildren		() const { return _children.count() > 0; }

	friend QTextStream &		operator <<		(QTextStream &stream, IdableWithChildren &idableWithChildren);

	friend QDebug			operator <<		(QDebug debug, IdableWithChildren &idableWithChildren);

protected:
	static QString			getListOfAnchors	(const QList <IdableWithChildren *> &iids,
								 const bool useFullName,
								 const bool createAnchor);
	QString				toStringRepresentationOfNumber (const int number,
									const QString singular,
									const QString plural) const;
	QTextStream &			notesToGEDCOM		(const QString &notes,
								 QTextStream &stream) const;
	QString				notesFromGEDCOM		(const QStringList &noteGedComList);


public:
	virtual QString			getAnchor		(const IDTYPE spouseIdForCurrentEntryOfFemale,
								 const bool useFullName,
								 const bool createAnchor);
	virtual QString			getListOfAnchors 	(const int type,
								 const bool useFullName,
								 const bool createAnchor);

	virtual bool			fullTextSearch		(const QRegExp &searchRegExp,
								 QStringList &resultsList) const;
};

#endif /* IDABLEWITHCHILDREN_H */
