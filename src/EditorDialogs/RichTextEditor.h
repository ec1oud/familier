#ifndef RICHTEXTEDITOR_H
#define RICHTEXTEDITOR_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QFormLayout>
#include <QDialogButtonBox>
#include "IconModel.h"
#include "FamilyModel.h"
#include "TextEditWithAutoComplete.h"

namespace Ui
{
	class RichTextEditor;
}

class RichTextEditor : public QWidget
{
	Q_OBJECT

	Ui::RichTextEditor *		ui;

public:
	explicit RichTextEditor(IconModel *iconModel, FamilyModel *familyModel, QWidget *parent = 0);
	~RichTextEditor();

	QString			toPlainText		();
	QString			toHtml			();

protected:
	void			changeEvent		(QEvent *e);

signals:
	void			textChanged		();

public slots:
	void			setText			(const QString &text);

private slots:
	void			setFontBoldened		();
	void			setFontItalicized	();
	void			setFontUnderlined	();
	void			setFontStriken		();
	void			setFontPointSize	(QString text);
	void			showColorDialog		();

	void			setAlignLeft		();
	void			setAlignRight		();
	void			setAlignCenter		();
	void			setAlignJustify		();

	void			insertHyperLink		();

	void			setTextChanged		();
};

class HyperLinkDialog : public QDialog
{
	Q_OBJECT

	QLabel			*labelHyperlink;
	QLineEdit		*lineEditHyperlink;
	QDialogButtonBox	*buttonBox;

public:
	explicit HyperLinkDialog (QWidget *parent = 0);

	QString			getHyperLink		() const;
	void			setHyperLink		(QString url);
};

#endif // RICHTEXTEDITOR_H
