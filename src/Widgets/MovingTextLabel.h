#ifndef MOVINGTEXTLABEL_H
#define MOVINGTEXTLABEL_H

#include <QLabel>
#include <QUrl>

class MovingTextLabel : public QLabel
{
	Q_OBJECT

	QString			_staticText;
	QString			_movingText;
	QString			_movingTextExpanded;
	int			_movingTextCurrentPosition;

public:
	explicit MovingTextLabel(QWidget *parent = 0);

	void		setStaticText		(const QString &);
	void		setMovingText		(const QString &);

signals:
	void		linkActivated		(const QUrl &link);

private slots:
	void		updateLabelText		();
	void		handleLinkHovering	(QString);
	void		handleLinkActivation	(QString);
};

#endif // MOVINGTEXTLABEL_H
