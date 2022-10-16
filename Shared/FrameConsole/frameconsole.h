#ifndef FRAMECONSOLE_H
#define FRAMECONSOLE_H

#include <QFrame>

namespace Ui {
	class FrameConsole;
}

class FrameConsole : public QFrame
{
	Q_OBJECT

public:
	explicit FrameConsole(QWidget *parent = nullptr);
	~FrameConsole();

	void add(const QString &text);

	void setFocus();

private slots:
	void on_lineEdit_returnPressed();

private:
	Ui::FrameConsole *ui;
	bool empty;
};

#endif // FRAMECONSOLE_H
