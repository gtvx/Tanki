#ifndef FRAMEREGISTRATION_H
#define FRAMEREGISTRATION_H

#include <QFrame>
#include <QTimer>

class Network;

namespace Ui {
	class FrameRegistration;
}

class FrameRegistration : public QFrame
{
	Q_OBJECT

public:
	explicit FrameRegistration(Network *network);
	~FrameRegistration();

	void init();
	void captcha(const QByteArray &data);
	void registrationNotExist();
	void registrationIncorrect();
	void registrationNicknameExist();
	void registrationNicknameNotMatchPattern();

private slots:
	void on_pushButton_clicked();
	void on_lineEdit_login_textChanged(const QString &arg1);
	void on_toolButton_clicked();

private:
	void timerOut();

	Ui::FrameRegistration *ui;
	Network *network;
	QTimer timer;
};

#endif // FRAMEREGISTRATION_H
