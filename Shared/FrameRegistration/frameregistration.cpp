#include "frameregistration.h"
#include "ui_frameregistration.h"
#include "networking/network.h"
#include <QImage>
#include <QDebug>


//registration;refresh_captcha

//register(param1:String, param2:String, param3:String, param4:Boolean, param5:String, param6:String, param7:String, param8:String):void
//this.network.send("registration;registration;" + param1 + ";" + param2 + ";" + param8;


//checkUid(param1:String):void
//this.network.send(("registration;check_name;" + param1));


//GoToRegistrationCommand
//send("registration;set_state");


FrameRegistration::FrameRegistration(Network *network) :
	QFrame(nullptr),
	ui(new Ui::FrameRegistration)
{
	ui->setupUi(this);
	this->network = network;
    connect(&timer, &QTimer::timeout, this, &FrameRegistration::timerOut);
	timer.setSingleShot(true);
	ui->pushButton->setEnabled(false);
}

FrameRegistration::~FrameRegistration()
{
	delete ui;
}

void FrameRegistration::init()
{
	this->show();
}

void FrameRegistration::captcha(const QByteArray &data)
{
	QPixmap pixmap;
	pixmap.loadFromData(data);
	ui->label_captcha->setPixmap(pixmap);
}

void FrameRegistration::registrationNotExist()
{
	ui->label_nick_status->clear();
	ui->pushButton->setEnabled(true);
}

void FrameRegistration::registrationIncorrect()
{
	ui->label_nick_status->setText("incorrect");
	ui->pushButton->setEnabled(false);
}

void FrameRegistration::registrationNicknameExist()
{
	ui->label_nick_status->setText("nickname exist");
	ui->pushButton->setEnabled(false);
}

void FrameRegistration::registrationNicknameNotMatchPattern()
{
	ui->label_nick_status->setText("nickname not match pattern");
	ui->pushButton->setEnabled(false);
}

void FrameRegistration::on_pushButton_clicked()
{
	QString login = ui->lineEdit_login->text();
	QString password_1 = ui->lineEdit_password_1->text();
	QString password_2 = ui->lineEdit_password_2->text();

	if (password_1 != password_2)
		return;

	if (password_1.length() < 4)
		return;

	QString captcha = ui->lineEdit_captcha->text();
	if (captcha.length() < 4)
		return;

	network->send("registration;registration;" + login + ";" + password_1 + ";" + captcha);
}

//_SafeStr_1870(param1:String, param2:String, param3:String, param4:Boolean, param5:String, param6:String, param7:String, param8:String):void
//register(param1, param2, param3, param4, param5, param6, param7, param8);

//this.serverFacade._SafeStr_1870(this.event.callsign, this.event.password, this.urlParams.domain, this.event.rememberMe, this.urlParams.referralHash, this.event.realName, this.event.idNumber, this.event.captchaAnswer);



void FrameRegistration::on_lineEdit_login_textChanged(const QString &arg1)
{
	(void)arg1;
	timer.start(1000);
}

void FrameRegistration::timerOut()
{
	QString login = ui->lineEdit_login->text();
	network->send("registration;check_name;" + login);
}


void FrameRegistration::on_toolButton_clicked()
{
	network->send("registration;refresh_captcha");
}

