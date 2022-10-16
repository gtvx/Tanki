#include <QDir>
#include <QCommandLineParser>
#include <QDebug>

#ifdef QT_GUI_LIB
#include <QApplication>
#include "mainwindow/mainwindow.h"
#else
#include <QCoreApplication>
#include "mainwindow/no_gui_mainwindow.h"
#endif


#include "networking/network.h"
#include "service.h"
#include "alternativa/tanks/model/panel/panelmodel.h"
#include "resource/resourceutil.h"
#include "_global.h"
#include "enterbattle.h"
#include "settings.h"
#include "get_dir.h"
#include "__tank_bot.h"


bool load_config();


int main(int argc, char *argv[])
{

#ifdef QT_GUI_LIB
	QApplication a(argc, argv);
	QPalette pal = a.palette();
	pal.setColor(QPalette::Window, QColor(240, 240, 240));
	a.setPalette(pal);
#else
	QCoreApplication a(argc, argv);
#endif

	try
	{
		QDir::setCurrent(a.applicationDirPath());
		if (!load_config())
			return -1;

		Settings settings;

		ResourceUtil::loadResource();

		EnterBattle enterBattle;

		{
			QCommandLineParser parser;

			QCommandLineOption command_login("login", "", "-");
			QCommandLineOption command_battle_name("battle_name", "", "-");
			QCommandLineOption command_battle_team("battle_team", "", "-");
			QCommandLineOption command_antialiasing("antialiasing", "", "-", "0");
#ifdef TANK_BOT
			QCommandLineOption command_bot("bot", "", "-", "0");
			QCommandLineOption command_bot_shoot("bot_shoot", "", "-", "1");
			QCommandLineOption command_bot_carry_flag("bot_carry_flag", "", "-", "1");
			QCommandLineOption command_bot_capture_points("bot_capture_points", "", "-", "1");
			QCommandLineOption command_bot_auto_kill("bot_auto_kill", "", "-", "1");
			QCommandLineOption command_bot_random_path("bot_random_path", "", "-", "0");
			QCommandLineOption command_bot_target_tank("bot_target_tank", "", "-", "1");
			QCommandLineOption command_bot_body_turn("bot_body_turn", "", "-", "1");
#endif

			parser.addOption(command_login);
			parser.addOption(command_battle_name);
			parser.addOption(command_battle_team);
			parser.addOption(command_antialiasing);

#ifdef TANK_BOT
			parser.addOption(command_bot);
			parser.addOption(command_bot_shoot);
			parser.addOption(command_bot_carry_flag);
			parser.addOption(command_bot_capture_points);
			parser.addOption(command_bot_auto_kill);
			parser.addOption(command_bot_random_path);
			parser.addOption(command_bot_target_tank);
			parser.addOption(command_bot_body_turn);
#endif

			parser.process(a);

			QString login_password = parser.value(command_login);
			QString battle_name = parser.value(command_battle_name);
			QString battle_team = parser.value(command_battle_team);

			settings.antialiasing = parser.value(command_antialiasing).toInt();

#ifdef TANK_BOT
			settings.bot = parser.value(command_bot).toInt();
			settings.bot_shoot = parser.value(command_bot_shoot).toInt();
			settings.bot_carry_flag = parser.value(command_bot_carry_flag).toInt();
			settings.bot_capture_points = parser.value(command_bot_capture_points).toInt();
			settings.bot_auto_kill = parser.value(command_bot_auto_kill).toInt();
			settings.bot_random_path = parser.value(command_bot_random_path).toInt();
			settings.bot_target_tank = parser.value(command_bot_target_tank).toInt();
			settings.bot_body_turn = parser.value(command_bot_body_turn).toInt();
#endif

			if (!battle_name.isEmpty()) {
				battle_name = QByteArray::fromBase64(battle_name.toLocal8Bit());
				enterBattle.setName(battle_name);

				if (!battle_team.isEmpty()) {
					BattleTeamType type;
					if (BattleTeamType_fromType(battle_team, &type)) {
						enterBattle.setTeam(type);
					}
				}
			}


			QString login, password;
			if (!login_password.isEmpty()) {
				QStringList list = login_password.split(QChar(':'));
				if (list.length() != 2)
					return -1;
				login = list[0];
				password = list[1];
			}

			if (login.isEmpty() || password.isEmpty()) {
				QFile file(get_dir() + "/login.txt");
				if (!file.open(QIODevice::ReadOnly)) {
					qDebug("not found login.txt");
					return -1;
				}

				QTextStream stream(&file);
				QString line = stream.readLine();
				QStringList list = line.split(QChar(':'));
				if (list.count() == 2) {
					login = list[0];
					password = list[1];
				}
			}

			if (login.isEmpty() || password.isEmpty()) {
				qDebug("no login");
				return -1;
			}


			settings.login = login;
			settings.password = password;
		}


#ifdef QT_GUI_LIB
		MainWindow w(&settings);
		w.show();
#else
		NoGuiMainWindow w(&settings);
#endif

		Service::setIMain(&w);
		w.battleEntrance(&enterBattle);


		PanelModel *panelModel = new PanelModel;
		Service::setPanelModel(panelModel);

		w.init();



		Network *network = new Network(&settings);
		Service::setNetworker(network);
		network->connect();

		int c = a.exec();

		Service::setIMain(nullptr);

		return c;
	}
	catch(int e)
	{
		qDebug() << "error main" << e;
	}

	return -1;
}
