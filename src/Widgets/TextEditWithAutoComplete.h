#ifndef TEXTEDITWITHAUTOCOMPLETE_H
#define TEXTEDITWITHAUTOCOMPLETE_H

#include <QTextEdit>
#include <QCompleter>
#include "IconModel.h"
#include "FamilyModel.h"

class TextEditWithAutoComplete : public QTextEdit
{
	Q_OBJECT

	bool				_isCompleteAndSelected;
	QCompleter *			_individualCompleter;
	QCompleter *			_familyCompleter;

public:
	explicit TextEditWithAutoComplete(QWidget *parent = 0);

	void			createCompleter		(IconModel *iconModel,
							 FamilyModel *familyModel);
	QString			toHtml			() const;

private:
	void			performCompletion	(bool isIndividualCompletion);
	void			insertCompletion	(bool isIndividualCompletion,
							 const QModelIndex &index,
							 bool singleWord = true);

protected:
	virtual void		keyPressEvent		(QKeyEvent *event);
	virtual void		mousePressEvent		(QMouseEvent *event);

private:
	bool			handledCompletedAndSelected (QKeyEvent *event);

private slots:
	void			performIndividualCompletion ();
	void			performFamilyCompletion	();
	void			insertIndividualCompletion
							(const QModelIndex &index,
							 bool singleWord = true);
	void			insertFamilyCompletion	(const QModelIndex &index,
							 bool singleWord = true);
	void			highlightCurrentLine	();

signals:

public slots:

};

#endif // TEXTEDITWITHAUTOCOMPLETE_H
