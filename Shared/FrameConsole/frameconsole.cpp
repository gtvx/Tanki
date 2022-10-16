#include "frameconsole.h"
#include "ui_frameconsole.h"
#include "service.h"
#include "networking/network.h"
#include <QDebug>


FrameConsole::FrameConsole(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::FrameConsole)
{
	ui->setupUi(this);
	this->empty = true;
}

FrameConsole::~FrameConsole()
{
	delete ui;
}


void FrameConsole::on_lineEdit_returnPressed()
{
	QString command = ui->lineEdit->text();
	bool ok = false;
	bool clear = true;

	if (command == "clear") {
		clear = true;
	} else if (command.startsWith("accept_package: ")) {
		ok = true;
		clear = false;

		QString s = command.mid(16);
		QByteArray data = s.toUtf8();
		Network *n = Service::getNetworker();
		if (n != nullptr)
			n->acceptPackage(data);
	}

	(void)ok;

	if (clear == true)
	{
		ui->lineEdit->clear();
		empty = true;
	}
}

void FrameConsole::add(const QString &text)
{
	if (text.isEmpty() == true)
		return;

	QTextCursor cursor = ui->plainTextEdit->textCursor();
	cursor.movePosition(QTextCursor::End);

	if (empty == false) {
		cursor.insertText(QChar('\n'));
	}

	cursor.insertText(text);

	empty = false;
}

void FrameConsole::setFocus()
{
	ui->lineEdit->setFocus();
}
