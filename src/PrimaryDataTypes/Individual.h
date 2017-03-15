#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "Familier.h"
#include "Union.h"
#include <QPixmap>
#include <QIcon>

/* The GETFUNCTIONI and SETFUNCTIONI macros were required as in case the Individual has been adopted
 * we need to get and set details, to and from the latest/last Individual entry and not here
 */

#define GETFUNCTIONI(class,function,type,argument) \
	GETFUNCTIONSIGNATURE(function,type) \
	{ \
		if (_adoptedTo) \
			return (dynamic_cast<class *> (_adoptedTo))->get ## function (); \
		GETFUNCTIONBODY(argument); \
	}

#define SETFUNCTIONI(class,function,type,argument) \
	SETFUNCTIONSIGNATURE(class,function,type,argument) \
	{ \
		if (_adoptedTo) \
			return (dynamic_cast<class *> (_adoptedTo))->set ## function (argument); \
		SETFUNCTIONBODY(class,type,argument) \
	}

#define SETFUNCTIONIQSTRING(class,function,argument) \
	SETFUNCTIONSIGNATURE(class,function,QString &,argument) \
	{ \
		if (_adoptedTo) \
			return (dynamic_cast<class *> (_adoptedTo))->set ## function (argument); \
		SETFUNCTIONBODYFORQSTRING(class,argument) \
	}

#define SETFUNCTIONWITHCONSTCASTI(class,function,type,argument) \
	SETFUNCTIONSIGNATURE(class,function,type,argument) \
	{ \
		if (_adoptedTo) \
			return (dynamic_cast<class *> (_adoptedTo))->set ## function (argument); \
		SETFUNCTIONBODYWITHCONSTCAST(class,type,argument) \
	}

/*
 * The children (_children) QList is the list
 * of Marriage Unions for the Individual
 */

class Family;

class NoBirthEventException : public std::exception
{
	IDTYPE _id;

public:
	NoBirthEventException (IDTYPE id)
	{
		_id = id;
	}

	IDTYPE getID () const { return _id; }
};

class Individual : public Familier
{
	DECLAREAUTOITEMMANAGEMENT(Individual)

public:
	static uint				s_DummyIndividualCount;

public:
	enum AnchorType { bornTo, spouses, spousesWithOffspring, siblings };

protected:
	bool					_dummy;
	bool					_dead;
	Individual *				_adoptedTo;
	Individual *				_adoptedFrom;

	Union *					_parentUnion;
	Event *					_birthEvent;
	Event *					_deathEvent;
	Family *				_family;
	bool					_isFamilyInherited;

	QString					_village;
	bool					_isVillageInherited;

	QString					_address;
	bool					_isAddressInherited;

	QString					_imagePath;
	QString					_facebookID;

	QString					_notes;

private:
	QIcon *					_thumbnail;
	QPixmap *				_pixmap;

public:
	Individual (const IDTYPE id,
		    const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
		    const unsigned long modificationCount);
	Individual (const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
		    const unsigned long modificationCount);
	Individual (const QDomElement &domElement);
	Individual (const Individual &individual);
	virtual ~Individual ();
	virtual void setupItemLinks (const QDomElement &domElement);
	//virtual bool isValid () const;
	virtual IdableWithChildren* getItemWithId (const IDTYPE id) const;
	virtual Individual* adoptAndReturnNewIndividual () = 0;

protected:
	void updateAdoptedIndividual (Individual *adoptedIndividual);

private:
	void init ();
	void initLinks ();

public:
	/* For isDummy () function we return directly from this object even if it has been adopted */
	bool			isDummy			() const { return _dummy; }
	bool			isNonDummy		() const { return !isDummy (); }
	bool			isDead			() const;
	bool			isAlive			() const { return !isDead (); }
	virtual bool		isMale			() const = 0;
	virtual bool		isFemale 		() const = 0;
	bool			isFamilyInherited	() const;
	bool			isVillageInherited	() const;
	bool			isAddressInherited	() const;

	GETFUNCTION		(AdoptedTo,		Individual*,		adoptedTo)
	GETFUNCTION		(AdoptedFrom,		Individual*,		adoptedFrom)
	bool			hasFather		() const { return getFather (); }
	bool			hasMother		() const { return getMother (); }
	Individual *		getFather		() const;
	Individual *		getMother		() const;
	GETFUNCTIONI		(Individual, ParentUnion,	Union *,		parentUnion)
	GETFUNCTIONI		(Individual, BirthEvent,	Event *,		birthEvent)
	GETFUNCTIONI		(Individual, DeathEvent,	Event *,		deathEvent)
	GETFUNCTIONI		(Individual, Family,		Family *,		family)
	GETFUNCTIONI		(Individual, Village,		QString,		village)
	GETFUNCTIONI		(Individual, Address,		QString,		address)
	QString			getImagePath		() const;
	GETFUNCTIONI		(Individual, FacebookID,	QString,		facebookID)
	GETFUNCTIONI		(Individual, Notes,		QString,		notes)

	GETSETMAXIDHEAD (Individual)

	SETFUNCTIONDECL		(AdoptedTo,		Individual *,	adoptedTo)
	SETFUNCTIONDECL		(AdoptedFrom,		Individual *,	adoptedFrom)
	SETFUNCTIONDECL		(Dummy,			bool,		dummy)
	SETFUNCTIONDECL		(Dead,			bool,		dead)
	SETFUNCTIONDECL		(ParentUnion,		Union *,	parentUnion)
	SETFUNCTIONDECL		(BirthEvent,		Event *,	birthEvent)
	SETFUNCTIONDECL		(DeathEvent,		Event *,	deathEvent)
	SETFUNCTIONDECL		(Family,		Family *,	family)
	SETFUNCTIONDECL		(VillageInherited,	bool,		villageInherited)
	SETFUNCTIONDECL		(Village,		QString &,	village)
	SETFUNCTIONDECL		(AddressInherited,	bool,		addressInherited)
	SETFUNCTIONDECL		(Address,		QString &,	address)
	SETFUNCTIONDECL		(ImagePath,		QString &,	imagePath)
	SETFUNCTIONDECL		(FacebookID,		QString &,	facebookID)
	SETFUNCTIONDECL		(Notes,			QString &,	notes)

	virtual Qt::ItemFlags	flags			(const int col, const bool isSpouseItem) const;
	virtual QVariant	data			(const int col, const int role, const IDTYPE spouseID);
	virtual bool		setData			(const int col, const QVariant &value, const IDTYPE spouseID);

	virtual QString		getChildTypeName	() const;

	friend QTextStream &	operator <<		(QTextStream &stream, Individual &individual);
	QTextStream &		toGEDCOM		(QTextStream &stream) const;
	void			fromGEDCOM		(const QStringList &gedcomEntryStringList,
							 const QHash<QString, Union*> &xRefToUnionHash,
							 const QHash <QString, QStringList> &xRefToNoteGEDCOMInfoHash);

	friend QDebug		operator <<		(QDebug debug, Individual &individual);

	QString			getPetName		() const;
	virtual QString		getSurName		() const;
	virtual QString		getFullName		() const;

	bool			hasImage		() const;
	QPixmap *		getImage		();
	QIcon *			getIcon			();

protected:
	static QString		getListOfAnchors	(const QList <Individual*> &individuals,
							 const bool useFullName,
							 const bool createAnchor);

private:
	QString			getDecoration		(const int col, const IDTYPE spouseID) const;

public:
	bool			setPetName		(const QString &petName);

	bool 			hasSpouse 		() const;
	bool			isSpouse		(const IDTYPE spid) const;
	long			spouseCount 		() const;
	QString			spouseCountAsString	() const;
	long			spouseNumber		(const IDTYPE spid) const;
	Individual*		getSpouse		(const long spidx) const;
	Individual*		getSpouseWithID		(const IDTYPE spid) const;
	Union*			getUnionWithSpouseID	(const IDTYPE spid) const;
	bool			hasOffspring		() const;
	bool			isOffspring		(const IDTYPE chid, const IDTYPE spid = 0) const;
	long			offspringCount		(const IDTYPE spid = 0) const;
	QString			offspringCountAsString	(const IDTYPE spid = 0) const;
	long			offspringNumber		(const IDTYPE spid, const IDTYPE chid) const;
	Individual*		getOffspring		(const long chidx, const IDTYPE spid) const;
	Individual*		getOffspringWithId	(const IDTYPE chid, const IDTYPE spid = 0) const;
	Event*			getMarriageEvent	(const IDTYPE spid) const;
	Event*			getDivorceEvent		(const IDTYPE spid) const;
	Event*			getMarriageEventIndex	(const long spidx) const;
	Event*			getDivorceEventIndex	(const long spidx) const;
	bool			setMarriageEvent	(const IDTYPE spid, Event *marriageEvent);
	bool			setDivorceEvent		(const IDTYPE spid, Event *divorceEvent);

	bool			hasSibling		() const;
	bool			hasElderSibling		() const;
	bool			hasYoungerSibling	() const;
	long			siblingCount		() const;
	QString			siblingCountAsString	() const;
	long			lastElderSibling	() const;
	long			firstYoungerSibling	() const;
	Individual *		getSibling		(const long sbidx) const;

	static bool		isOlderThan		(const Individual *left,
							 const Individual *right);
	static bool		isYoungerThan		(const Individual *left,
							 const Individual *right);
	bool			isOlderThan		(const Individual *individual) const;


	virtual QString		genderBias		(const Familier::EnglishWord ew,
							 const bool capitalise) const = 0;
	virtual QString		deathBias		(const Familier::EnglishWord ew,
							 const bool capitalise) const;
	virtual QString		getAnchor		(const IDTYPE spouseIdForCurrentEntryOfFemale,
							 const bool useFullName,
							 const bool createAnchor);
	virtual QString		getListOfAnchors	(const int type, const bool useFullName,
							 const bool createAnchor);

	virtual QString		getToolTip		(const bool createAnchor,
							 const IDTYPE spouseIdForCurrentEntryOfFemale);
	virtual QString		getItemViewPage		(const IDTYPE spouseIdForCurrentEntryOfFemale);
	virtual bool		fullTextSearch		(const QRegExp &searchRegExp,
							 QStringList &resultsList) const;

	QList <Individual*>	getElderBrothers	() const;
	QList <Individual*>	getYoungerBrothers	() const;
	QList <Individual*>	getBrothers		() const;
	QList <Individual*>	getSisters		() const;
	QList <Individual*>	getSiblings		() const;

	QList <Individual*>	getElderMaleCousins	(bool &allok) const;
	QList <Individual*>	getYoungerMaleCousins	(bool &allok) const;
	QList <Individual*>	getMaleCousins		(bool &allok) const;
	QList <Individual*>	getFemaleCousins	(bool &allok) const;
	QList <Individual*>	getCousins		(bool &allok) const;

	QList <Individual*>	getJijaBhabhis		(bool &allok) const;
	QList <Individual*>	getStepParents		() const;
	QList <Individual*>	getHalfSiblngs		() const;
	QList <Individual*>	getDadaDadis		(bool &allok) const;

	QList <Individual*>	getTauTais		(bool &allok) const;
	QList <Individual*>	getChachaChachis	(bool &allok) const;
	QList <Individual*>	getChachereCousins	(bool &allok) const;
	QList <Individual*>	getFufaBuas		(bool &allok) const;
	QList <Individual*>	getFufereCousins	(bool &allok) const;

	QList <Individual*>	getBhatijaBhatijis	(bool &allok) const;
	QList <Individual*>	getBhanjaBhanjis	(bool &allok) const;

	QList <Individual*>	getNanaNanis		(bool &allok) const;

	QList <Individual*>	getMamaMamis		(bool &allok) const;
	QList <Individual*>	getMamereCousins	(bool &allok) const;

	QList <Individual*>	getMausaMausis		(bool &allok) const;
	QList <Individual*>	getMausereCousins	(bool &allok) const;

	QList <Individual*>	getSpouses		() const;
	QList <Individual*>	getOffsprings		() const;
	QList <Individual*>	getDamadsAndBahus	() const;
	QList <Individual*>	getPotaPotis		() const;
	QList <Individual*>	getNaatiNaatins		() const;

	QList <Individual*>	getSaasSasurs		() const;

	virtual QList <Individual*>	getSalaSalhajs	(bool &allok) const = 0;
	virtual QList <Individual*>	getSaadhuSalis	(bool &allok) const = 0;

	virtual QList <Individual*>	getJethJethanis	(bool &allok) const = 0;
	virtual QList <Individual*>	getDevarDevarani(bool &allok) const = 0;
	virtual QList <Individual*>	getNanadNandoi	(bool &allok) const = 0;

	QString			getRelatives		(bool &listMayBeIncomplete) const;

protected:
	void			getGrandFatherMothers	(const Individual *fatherOrMother,
							 QList <Individual *> &retList,
							 bool &allok) const;
	QList <Individual*>	getSiblingOffsprings	(bool female, bool &allok) const;
	QList <Individual*>	getGrandChildren	(bool female) const;
	QList <Individual*>	getOlderOrYoungCousins	(bool older, bool &allok) const;
	QString			getRelativesAsDisplayString () const;
	void			getRelativesHelperFunction
							(const QString maleRelation,
							 const QString femaleRelation,
							 const QList <Individual *> relatives,
							 QList <QPair <QString, Individual*> > &relationList) const;
	void			addIndividualAndSpouse	(const Individual *individual,
							 QList <Individual *> &retList) const;
	void			addIndividualAndSpouseForMale
							(const Individual *individual,
							 QList <Individual *> &retList) const;
	void			addIndividualAndSpouseForFemale
							(const Individual *individual,
							 QList <Individual *> &retList) const;
	void			addChildrenForUncleOrAunt
							(const Individual *uncleAunt,
							 QList <Individual *> &retList,
							 bool forUncle = true) const;
	void			addChildrenForUncle	(const Individual *uncleAunt,
							 QList <Individual *> &retList) const;
	void			addChildrenForAunt	(const Individual *uncleAunt,
							 QList <Individual *> &retList) const;
};

#endif /* INDIVIDUAL_H */
