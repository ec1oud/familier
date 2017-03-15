#include "MovingTextLabel.h"
#include <QTimer>
#include <QMouseEvent>
#include <QToolTip>
#include "Utilities.h"
#include "Individual.h"
#include "Family.h"

MovingTextLabel::MovingTextLabel(QWidget *parent) :
		QLabel(parent), _movingTextCurrentPosition (0)
{
	QTimer *timer = new QTimer (this);
	connect (timer, SIGNAL(timeout()), this, SLOT(updateLabelText()));
	timer->start (400);

	setFixedWidth (400);
	setMouseTracking (true);

	connect (this, SIGNAL(linkHovered(QString)), this, SLOT(handleLinkHovering(QString)));
	connect (this, SIGNAL(linkActivated(QString)), this, SLOT(handleLinkActivation(QString)));
}

void MovingTextLabel::setStaticText (const QString &staticText)
{
	_staticText = staticText;
	updateLabelText ();
}

void MovingTextLabel::setMovingText (const QString &movingText)
{
	_movingText = movingText;
	_movingTextCurrentPosition = 0;
	updateLabelText ();
}

void MovingTextLabel::updateLabelText ()
{
	QFontMetrics fm (font ());
	QString movingText;

	QString plainMovingText = Utilities::convertFTMLToDisplayFormat (_movingText, movingText,
									 true, true,
									 _movingTextCurrentPosition);

	setText (_staticText + movingText);
	if (fm.width (_staticText + plainMovingText) >= 400)
	{
		_movingTextCurrentPosition ++;
		if (_movingTextCurrentPosition >= plainMovingText.length ())
			_movingTextCurrentPosition = 0;
	}
}

void MovingTextLabel::handleLinkHovering (QString link)
{
	IDTYPE id = 0;
	bool isIndividual = false;

	QUrl url (link);
	if (url.scheme() == "iid")
	{
		id = url.path().toUInt();
		isIndividual = true;
	}
	else if (url.scheme() == "fid")
	{
		id = url.path().toUInt();
		isIndividual = false;
	}

	if (id)
	{
		if (isIndividual)
			QToolTip::showText (QCursor::pos(), Individual::s_IdToIndividualHash.value (id)->getToolTip (false, 0));
		else
			QToolTip::showText (QCursor::pos(), Family::s_IdToFamilyHash.value (id)->getToolTip (false, 0));
	}
}

void MovingTextLabel::handleLinkActivation (QString link)
{
	QUrl url (link);
	emit linkActivated (url);
}
