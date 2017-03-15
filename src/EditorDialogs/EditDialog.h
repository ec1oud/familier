#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include "Familier.h"
#include "StringListModel.h"
#include <QListView>

class EditDialog : public QDialog
{
	Q_OBJECT

	bool				_edited;

protected:
	QListView *			_aliasesListView;
	StringListModel *		_aliasesModel;

public:
	explicit EditDialog(QStringList aliases, QWidget *parent = 0);
	~EditDialog();

	bool		isEdited			() const { return _edited; }

private:
	virtual void	setupUi				() = 0;

public:
	virtual bool	save				() = 0;

signals:

protected slots:
	virtual void	setEdited			();
	void		addAlias			();
	void		delAlias			();
	void		movAliasUp			();
	void		movAliasDn			();
};

#endif // EDITDIALOG_H
