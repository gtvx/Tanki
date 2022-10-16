#include "sendpackage.h"
#include "ui_sendpackage.h"
#include "service.h"
#include "networking/network.h"

static void Network_send(const QString &str)
{
	Network *network = Service::getNetworker();
	if (network != nullptr)
		network->send(str);
}

void show_send_package()
{
	new SendPackage(nullptr);
}



SendPackage::SendPackage(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SendPackage)
{
	ui->setupUi(this);
	connect(&timer, &QTimer::timeout, this, &SendPackage::send_time);
	timer.setTimerType(Qt::PreciseTimer);
	this->show();
}

SendPackage::~SendPackage()
{
	delete ui;
}

void SendPackage::on_pushButton_clicked()
{
	send();
}

void SendPackage::on_lineEdit_returnPressed()
{
	send();
}

void SendPackage::closeEvent(QCloseEvent*)
{
	delete this;
}

void SendPackage::on_checkBox_clicked(bool checked)
{
	if (checked == true) {
		timer.start(ui->spinBox->value());
	} else {
		timer.stop();
	}
}

void SendPackage::send()
{
	QStringList list = ui->lineEdit->text().split("<Separation>");
	for (int i = 0; i < list.count(); i++)
		Network_send(list[i]);
}

void SendPackage::send_time()
{
	send();
	timer.start(); //for reset, otherwise interval < timer.interval().
}
