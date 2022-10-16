#ifndef SENDPACKAGE_H
#define SENDPACKAGE_H

#include <QWidget>
#include <QTimer>

namespace Ui {
	class SendPackage;
}

class SendPackage : public QWidget
{
	Q_OBJECT

public:
	explicit SendPackage(QWidget *parent = nullptr);
	~SendPackage();

private slots:
	void on_pushButton_clicked();
	void on_lineEdit_returnPressed();
	void on_checkBox_clicked(bool checked);

private:
	Ui::SendPackage *ui;
	QTimer timer;
	//uint32_t timer_interval;
	//uint64_t time_last_send;
	void closeEvent(QCloseEvent *event);
	void send();
	void send_time();
};


#endif // SENDPACKAGE_H
