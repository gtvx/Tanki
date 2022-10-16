#include "usermodel.h"
#include "networking/commands/command.h"
#include "service.h"
#include "networking/network.h"
#include "networking/networkbytes.h"
#include "lobby.h"
#include "IMain.h"
#include "settings.h"
#include "__global.h"


UserModel::UserModel(Network *network)
{
	this->network = network;

	network->addListener(this);
	const Settings *settings = network->getSettings();
	auth(settings->login, settings->password, "");

	//Service::getIMain()->showRegistration(network);
	//network->send("registration;set_state");
}

UserModel::~UserModel()
{

}

bool UserModel::onData(Command *data)
{
	if (data->type == CommandType::REGISTRATON) {
		QByteArray cmd = data->readArgument();

		if (cmd == "check_name_result") {
			QByteArray a = data->readArgument();
			if (a == "not_exist") {
				Service::getIMain()->registrationNotExist();
			} else if (a == "incorrect") {
				Service::getIMain()->registrationIncorrect();
			} else if (a == "nickname_exist") {
				//nickname_exist;{"free_nicknames":["qwerty_fx","qwerty_super","qwerty_DVD","qwerty_master","qwerty_power"]}
				Service::getIMain()->registrationNicknameExist();
			} else if (a == "nickname_not_match_pattern") {
				Service::getIMain()->registrationNicknameNotMatchPattern();
			}
		}



		return false;

	} else if (data->type == CommandType::AUTH) {
		QByteArray cmd = data->readArgument();
		if (cmd == "accept") {
			network->removeListener(this);

			Lobby *lobby = new Lobby(network);
			Service::setLobby(lobby);
			network->addListener(lobby);
#ifdef L_PRO
			network->send("lobby;showDamageEnabled;true");
#endif
			delete this;

		} else if (cmd == "denied") {
			Service::getIMain()->wrongPassword();
		} else if (cmd == "not_exist") {

		} else if (cmd == "ban") {
			Service::getIMain()->ban();
		} else if (cmd == "recovery_account_done") {

		} else if (cmd == "wrong_captcha") {

		} else if (cmd == "recovery_account_code") {

		} else if (cmd == "enable_captcha") {
			Service::getIMain()->captcha();
		} else if (cmd == "update_captcha") {
			//auth;update_captcha;REGISTER;-119,8
			QByteArray a = data->readArgument();
			if (a == "REGISTER")
			{
				QByteArray captcha_values = data->readRest();
				QByteArray captcha = NetworkBytesFrom(captcha_values);
				Service::getIMain()->registrationCaptcha(captcha);
			}

		} else if (cmd == "recovery_account") {

		}

		return false;
	}

	return true;
}



void UserModel::auth(const QString &mail, const QString &password, const QString &captcha)
{		
#ifndef L_PRO
	QString data = "auth;" + mail + ";" + password;
	if (captcha.isEmpty() == false)
		data.append(";" + captcha);
#else
	//network->send("registration;set_state");


	QString data = "auth;login;" + captcha + ";false;" + mail + ";" + password;


	/*
	QString data;
	data.append("auth;login;{\"login\":\"");
	data.append(mail);
	data.append("\",\"password\":\"");
	data.append(password);
	data.append("\",\"captcha\":\"");
	data.append(captcha);
	data.append("\",\"remember\":false}");
	*/

	//auth;login;{"remember":true,"captcha":"","login":"","password":""}
	//send(((((((("auth;login;" + param4) + ";") + param3) + ";") + param1) + ";") + param2));
	//this.entranceGateway.login(this.loginEvent.callsign, this.loginEvent.password, this.loginEvent.rememberMe, this.loginEvent.captchaAnswer);

#endif


	network->send(data);
}

