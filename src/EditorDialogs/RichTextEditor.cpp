#include "RichTextEditor.h"
#include "ui_RichTextEditor.h"
#include <QColorDialog>
#include <QMessageBox>

RichTextEditor::RichTextEditor(IconModel *iconModel, FamilyModel *familyModel, QWidget *parent)
	: QWidget(parent), ui(new Ui::RichTextEditor)
{
	ui->setupUi(this);
	ui->textEdit->createCompleter (iconModel, familyModel);
	setWindowTitle ("Edit description");
}

RichTextEditor::~RichTextEditor()
{
	delete ui;
}

QString RichTextEditor::toPlainText ()
{
	return ui->textEdit->toPlainText ();
}

QString RichTextEditor::toHtml ()
{
	if (ui->textEdit->toPlainText ().length ())
	{
		QString notes = ui->textEdit->toHtml ();
		notes.replace (QRegExp("<!DOCTYPE HTML PUBLIC[^>]*>.*<body [^>]*>."), "");
		notes.replace (QRegExp("</body></html>"), "");
		return notes.trimmed ();
	}
	else
		return "";
}

void RichTextEditor::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void RichTextEditor::setText (const QString &text)
{
	ui->textEdit->setText (text);
}

void RichTextEditor::setFontBoldened ()
{
	QTextCharFormat format = ui->textEdit->currentCharFormat ();
	if (format.fontWeight () == QFont::Bold)
		format.setFontWeight (QFont::Normal);
	else
		format.setFontWeight (QFont::Bold);
	ui->textEdit->setCurrentCharFormat (format);
}

void RichTextEditor::setFontItalicized ()
{
	QTextCharFormat format = ui->textEdit->currentCharFormat ();
	format.setFontItalic (!format.fontItalic ());
	ui->textEdit->setCurrentCharFormat (format);
}

void RichTextEditor::setFontUnderlined ()
{
	QTextCharFormat format = ui->textEdit->currentCharFormat ();
	format.setFontUnderline (!format.fontUnderline ());
	ui->textEdit->setCurrentCharFormat (format);
}

void RichTextEditor::setFontStriken ()
{
	QTextCharFormat format = ui->textEdit->currentCharFormat ();
	format.setFontStrikeOut (!format.fontStrikeOut ());
	ui->textEdit->setCurrentCharFormat (format);
}

void RichTextEditor::setFontPointSize (QString text)
{
	ui->textEdit->setFontPointSize (text.toFloat ());
}

void RichTextEditor::showColorDialog ()
{
	QColor color = QColorDialog::getColor ();
	if (color.isValid ())
		ui->textEdit->setTextColor (color);
}

void RichTextEditor::setAlignLeft ()
{
	ui->textEdit->setAlignment (Qt::AlignLeft);
}

void RichTextEditor::setAlignRight ()
{
	ui->textEdit->setAlignment (Qt::AlignRight);
}

void RichTextEditor::setAlignCenter ()
{
	ui->textEdit->setAlignment (Qt::AlignCenter);
}

void RichTextEditor::setAlignJustify ()
{
	ui->textEdit->setAlignment (Qt::AlignJustify);
}

void RichTextEditor::insertHyperLink ()
{
	QTextCursor cursor = ui->textEdit->textCursor ();
	QString linkText = cursor.selectedText ();
	if (!linkText.length ())
	{
		QMessageBox::warning (this, "No Text Selected", "Invalid or no Text Selected. Please select some text to continue.");
		return;
	}
	QString url = ui->textEdit->currentCharFormat ().anchorHref ();
	HyperLinkDialog *dialog = new HyperLinkDialog(this);
	if (!dialog)
		return;

	dialog->setHyperLink (url);
	if (QDialog::Accepted == dialog->exec ())
	{
		url = dialog->getHyperLink ();
		QTextCharFormat currentTextCharFormat = ui->textEdit->currentCharFormat ();
		if (url.length ())
		{
			currentTextCharFormat.setAnchor (true);
			currentTextCharFormat.setAnchorHref (url);
		}
		else
			currentTextCharFormat.setAnchor (false);
		ui->textEdit->setCurrentCharFormat (currentTextCharFormat);
	}
	delete dialog;
}

void RichTextEditor::setTextChanged ()
{
	emit textChanged ();
}

HyperLinkDialog::HyperLinkDialog (QWidget *parent) : QDialog(parent)
{
	setWindowTitle ("Please provide a URL for Hyperlink");
	setWindowModality (Qt::WindowModal);
	resize(448, 136);
	setModal (true);
	QFormLayout *layout = new QFormLayout(this);
	labelHyperlink = new QLabel(this);
	labelHyperlink->setText ("URL:");
	layout->setWidget (0, QFormLayout::LabelRole, labelHyperlink);
	lineEditHyperlink = new QLineEdit(this);
	layout->setWidget (0, QFormLayout::FieldRole, lineEditHyperlink);
	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation (Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	layout->setWidget (1, QFormLayout::SpanningRole, buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString HyperLinkDialog::getHyperLink () const
{
	return lineEditHyperlink->text ();
}

void HyperLinkDialog::setHyperLink (QString url)
{
	lineEditHyperlink->setText (url);
}
