#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debug.h"
#include "alternativa/math/vector3.h"
#include "service.h"
#include "_global.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "lobby.h"
#include "alternativa/tanks/model/battleselectmodel.h"
#include "alternativa/tanks/model/panel/panelmodel.h"
#include "tank_bot/bot.h"
#include "tank_bot/botsettings.h"
#include "tank_bot/map_points.h"
#include "FrameLobby/framelobby.h"
#include "FrameGarage/graphics_framegarage.h"
#include "alternativa/init/garagemodelactivator.h"
#include "alternativa/tanks/model/garagemodel.h"
#include "FrameConsole/frameconsole.h"
#include "FrameRegistration/frameregistration.h"
#include "file.h"
#include "keys.h"
#include "__tank_bot.h"


#ifdef GRAPHICS
#include "FrameBattle/graphics_framebattle.h"
#else
#include "FrameBattle/framebattle.h"
#endif


#include "mymath.h"
#include "tanks.h"
#include "settings.h"
#include "keys.h"
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QDebug>



void show_send_package();




MainWindow::MainWindow(const Settings *settings, QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowTitle(" ");

	this->settings = settings;


	_hide = false;
	_renderEnable = true;


	frameLobby = nullptr;
	frameBattle = nullptr;
	frameGarage = nullptr;
	frameConsole = nullptr;
	frameRegistration = nullptr;


	ui->toolButton_c->setEnabled(false);
	ui->toolButton_p->setEnabled(false);
	ui->toolButton_c->hide();
	ui->toolButton_p->hide();
	ui->toolButton->setEnabled(false);
	ui->toolButton->hide();

	ui->checkBox_render_off->hide();


	ui->pushButton_garage->hide();
	ui->pushButton_exit->hide();



	bot = nullptr;
	menu_bot = nullptr;

#ifdef TANK_BOT
	botSettings = new BotSettings;
	botSettings->shoot = settings->bot_shoot;
	botSettings->carry_flag = settings->bot_carry_flag;
	botSettings->capture_points = settings->bot_capture_points;
	botSettings->auto_kill = settings->bot_auto_kill;
	botSettings->random_path = settings->bot_random_path;
	botSettings->target_tank = settings->bot_target_tank;
	botSettings->body_turn = settings->bot_body_turn;
	botSettings->shoot = settings->bot_shoot;


	menu_bot = new QMenu;

	{
		QAction *a = menu_bot->addAction("Shoot");
		a->setCheckable(true);
		a->setChecked(this->botSettings->shoot);
		connect(a, &QAction::triggered, this, [a, this] (bool checked) {
			this->botSettings->shoot = checked;
			if (bot != nullptr)
				bot->updateCommands();
		});
	}

	{
		QAction *a = menu_bot->addAction("Tank");
		a->setCheckable(true);
		a->setChecked(this->botSettings->target_tank);
		connect(a, &QAction::triggered, this, [a, this] (bool checked) {
			this->botSettings->target_tank = checked;
			if (bot != nullptr)
				bot->updateCommands();
		});
	}

	{
		QAction *a = menu_bot->addAction("Carry flag");
		a->setCheckable(true);
		a->setChecked(this->botSettings->carry_flag);
		connect(a, &QAction::triggered, this, [a, this] (bool checked) {
			this->botSettings->carry_flag = checked;
			if (bot != nullptr)
				bot->updateCommands();
		});
	}

	{
		QAction *a = menu_bot->addAction("Capture points");
		a->setCheckable(true);
		a->setChecked(this->botSettings->capture_points);
		connect(a, &QAction::triggered, this, [a, this] (bool checked) {
			this->botSettings->capture_points = checked;
			if (bot != nullptr)
				bot->updateCommands();
		});
	}

	{
		QAction *a = menu_bot->addAction("Random paths");
		a->setCheckable(true);
		a->setChecked(this->botSettings->random_path);
		connect(a, &QAction::triggered, this, [a, this] (bool checked) {
			this->botSettings->random_path = checked;
			if (bot != nullptr)
				bot->updateCommands();
		});
	}



	{
		QAction *a = menu_bot->addAction("Body turn");
		a->setCheckable(true);
		a->setChecked(this->botSettings->body_turn);
		connect(a, &QAction::triggered, this, [a, this] (bool checked) {
			this->botSettings->body_turn = checked;
			if (bot != nullptr)
				bot->updateCommands();
		});
	}

	{
		QAction *a = menu_bot->addAction("Auto kill");
		a->setCheckable(true);
		a->setChecked(this->botSettings->auto_kill);
		connect(a, &QAction::triggered, this, [a, this] (bool checked) {
			this->botSettings->auto_kill = checked;
			if (bot != nullptr)
				bot->updateCommands();
		});
	}

	{
		QAction *a = menu_bot->addAction("Draw points");
		a->setCheckable(true);
		a->setChecked(this->botSettings->draw_points);
		connect(a, &QAction::triggered, this, [a, this] (bool checked) {
			this->botSettings->draw_points = checked;
			if (bot != nullptr)
				bot->updateCommands();
		});
	}

	{
		QAction *a = menu_bot->addAction("Create points");
		connect(a, &QAction::triggered, this, [a, this] () {
			TankModel *tankModel = Service::getTankModel();
			if (tankModel != nullptr) {
				Vector3 &pos = tankModel->localUserData->tank->state.pos;
				if (this->bot != nullptr) {
					auto fix = [] (int32_t v) { return (v % 10) == 0 ? v + 1 : v; };
					this->bot->getMapPoints()->create(fix(pos.x), fix(pos.y), fix(pos.z));
					this->bot->savePoints();
					this->updateCountPoints();
				}
			}
		});
	}

	/*
	{
		QAction *a = menu_bot->addAction("Save points");
		connect(a, &QAction::triggered, this, [a, this] () {
			if (this->bot != nullptr)
				this->bot->savePoints();
		});
	}

	{
		QAction *a = menu_bot->addAction("Load points");
		connect(a, &QAction::triggered, this, [a, this] () {
			if (this->bot != nullptr)
				this->bot->loadPoints();
			this->updateCountPoints();
		});
	}
	*/

	if (settings->bot == true) {
		ui->checkBox_bot_enable->setChecked(true);
	}
#else
	ui->toolButton_bot->hide();
	ui->checkBox_bot_enable->hide();
	ui->label_map_points->hide();
#endif

	//showLobby();
	//showGarage();
}

MainWindow::~MainWindow()
{
#ifdef TANK_BOT
	delete menu_bot;
	delete botSettings;
#endif
	delete ui;
}

void MainWindow::getSize(int *width, int *height)
{
	if (frameBattle != nullptr) {
		frameBattle->getSize(width, height);
	} else {
		*width = 500;
		*height = 500;
	}
}

void MainWindow::initTank(Tank *tank)
{
	if (tank->tankData->local == true) {
		tank->botSettings = botSettings;
#ifdef TANK_BOT
		this->bot = tank->initBot();
		this->bot->setEnable(this->getBotEnable());
#endif
		if (frameBattle != nullptr)
			frameBattle->setBot(this->bot);
		this->updateCountPoints();
	}

	if (frameBattle != nullptr)
		frameBattle->initTank(tank);
}

void MainWindow::deleteTank(Tank *tank)
{
	if (tank == nullptr)
		return;
#ifdef TANK_BOT
	if (bot != nullptr) {
		bot->deleteTank(tank);
	}
#endif
	if (tank->tankData->local == true) {
		if (frameBattle != nullptr)
			frameBattle->setBot(nullptr);
		bot = nullptr;
	}
}

void MainWindow::updateCountPoints()
{
#ifdef TANK_BOT
	uint32_t length = this->bot != nullptr ? this->bot->getMapPoints()->length : 0;
	if (length != 0)
		ui->label_map_points->setText(QString::number(length));
	else
		ui->label_map_points->clear();
#endif
}


void MainWindow::createDamageEffect(Tank *tank, int damage, uint32_t color)
{
	if (frameBattle != nullptr)
		frameBattle->createDamageEffect(tank, damage, color);
}

bool MainWindow::getBotEnable()
{
	return ui->checkBox_bot_enable->isChecked();
}

void MainWindow::showSuicideIndicator()
{
#ifndef GRAPHICS
	if (frameBattle != nullptr)
		frameBattle->showSuicideIndicator();
#endif
}

void MainWindow::hideSuicideIndicator()
{
#ifndef GRAPHICS
	if (frameBattle != nullptr)
		frameBattle->hideSuicideIndicator();
#endif
}

void MainWindow::battleChatMessage(const BattleChatMessage &message)
{
	(void)message;
#ifndef GRAPHICS
	if (frameBattle == nullptr)
		return;
	frameBattle->battleChatMessage(message);
#endif
}

void MainWindow::battleEntrance(EnterBattle *enterBattle)
{
	this->_enterBattle = enterBattle;
	if (frameLobby != nullptr)
		frameLobby->battleEntrance(enterBattle);
}

void MainWindow::socketConnected()
{

}

void MainWindow::socketDisconnected()
{
	ui->label_status->setText("disconnected");
}

void MainWindow::noConnection()
{
	ui->label_status->setText("no connection");
}

void MainWindow::wrongPassword()
{
	ui->label_status->setText("wrong password");
}

void MainWindow::captcha()
{
	ui->label_status->setText("captcha");
}

void MainWindow::ban()
{
	ui->label_status->setText("ban");
}

void MainWindow::showLobby()
{
	hideGarage();
	hideBattle();
	if (frameLobby != nullptr)
		return;

	panel->setCurrentBattles();

	frameLobby = new FrameLobby;
	frameLobby->setRank(rank);
	frameLobby->battleEntrance(_enterBattle);
	ui->gridLayout_3->addWidget(frameLobby, 0, 0, 1, 1);
}

void MainWindow::hideLobby()
{
	if (frameLobby == nullptr)
		return;
	delete frameLobby;
	frameLobby = nullptr;
}

void MainWindow::showBattle()
{
	hideGarage();
	hideLobby();
	if (frameBattle != nullptr)
		return;

	panel->setCurrentBattle();

#ifdef GRAPHICS
	frameBattle = new GraphicsFrameBattle(Service::getNetworker());
#else
	frameBattle = new FrameBattle(Service::getNetworker());
#endif

	frameBattle->setSettings(this->settings);
	ui->gridLayout_3->addWidget(frameBattle, 0, 0, 1, 1);

	frameBattle->init();
}

void MainWindow::hideBattle()
{
	if (frameBattle == nullptr)
		return;
	delete frameBattle;
	frameBattle = nullptr;
}

void MainWindow::showGarage(GarageModel *garageModel)
{
	(void)garageModel;

	hideLobby();

	this->panel->setCurrentGarage();

#ifdef GRAPHICS
	if (frameGarage != nullptr)
		return;

	frameGarage = new GraphicsFrameGarage(Service::getNetworker());

	frameGarage->setSettings(this->settings);
	ui->gridLayout_3->addWidget(frameGarage, 0, 0, 1, 1);

	frameGarage->init(garageModel);
#endif
}


void MainWindow::hideGarage()
{
	if (frameGarage == nullptr)
		return;

	delete frameGarage;
	frameGarage = nullptr;
}


void MainWindow::showRegistration(Network *network)
{
	if (frameRegistration != nullptr)
		return;

	frameRegistration = new FrameRegistration(network);

	ui->gridLayout_3->addWidget(frameRegistration, 0, 0, 1, 1);

	frameRegistration->init();
}


void MainWindow::hideRegistration()
{
	if (frameRegistration == nullptr)
		return;

	delete frameRegistration;
	frameRegistration = nullptr;
}

void MainWindow::registrationNotExist()
{
	if (frameRegistration != nullptr)
		frameRegistration->registrationNotExist();

}

void MainWindow::registrationIncorrect()
{
	if (frameRegistration != nullptr)
		frameRegistration->registrationIncorrect();

}

void MainWindow::registrationNicknameExist()
{
	if (frameRegistration != nullptr)
		frameRegistration->registrationNicknameExist();
}

void MainWindow::registrationNicknameNotMatchPattern()
{
	if (frameRegistration != nullptr)
		frameRegistration->registrationNicknameNotMatchPattern();
}


void MainWindow::paintEvent(QPaintEvent *event)
{
	(void)event;
}




void MainWindow::resizeEvent(QResizeEvent *event)
{
	(void)event;
	/*
	BattlefieldModel *battle = Service::getBattlefieldModel();
	if (battle == nullptr)
		return;
	GameCamera *camera = battle->bfData->viewport->camera;

	if (frameBattle != nullptr) {
		int width, height;
		frameBattle->getSize(&width, &height);
		camera->setViewSize(width, height);
		battle->bfData->viewport->resize(width, height);
	}
	*/
}

void MainWindow::draw()
{
	if (frameBattle != nullptr)
		frameBattle->draw();
}



void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
		return;
#if WIN32
	int key = event->nativeVirtualKey();
#else
	int key = scan_code_to_key(event->nativeScanCode());
#endif

	if (key == KEY_F6)
	{
		if (frameConsole == nullptr)
			showConsole();
		else
			hideConsole();
	}
	else if (key == KEY_F10)
	{
		if (ui->frame->isHidden()) {
			ui->frame->show();
		} else {
			ui->frame->hide();
		}
	}


	TankModel *tankModel = Service::getTankModel();
	if (tankModel == nullptr)
		return;


	tankModel->onKey(key, true);
#ifndef GRAPHICS
	if (key == KEY_RETURN) {
		if (frameBattle != nullptr)
			frameBattle->enter();
	}
#endif
}



void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
		return;
#if WIN32
	int key = event->nativeVirtualKey();
#else
	int key = scan_code_to_key(event->nativeScanCode());
#endif


	TankModel *tankModel = Service::getTankModel();
	if (tankModel == nullptr)
		return;

	tankModel->onKey(key, false);
}

void MainWindow::setNick(const QString &nick)
{
	ui->label_nick->setText(nick);
}

void MainWindow::setRank(int rank)
{
	this->rank = rank;
	ui->label_rank->setText(QString::number(rank));
	if (frameLobby != nullptr)
		frameLobby->setRank(rank);
}

void MainWindow::setCrystal(int crystal)
{
	ui->label_crystal->setText(QString::number(crystal));
}

void MainWindow::clearBattles()
{
	if (frameLobby != nullptr)
		frameLobby->clearBattles();
}

void MainWindow::addBattle(const BattleClient &battle)
{
	if (frameLobby != nullptr)
		frameLobby->addBattle(battle);
}

void MainWindow::showBattleInfo(const ShowBattleInfo &b)
{
	if (frameLobby != nullptr)
		frameLobby->showBattleInfo(b);
}


void MainWindow::addPlayerToBattle(const QString &battleId, const UserInfoClient &user)
{
	if (frameLobby != nullptr)
		frameLobby->addPlayerToBattle(battleId, user);
}

void MainWindow::removePlayerFromBattle(const QString &battleId, const QString &id)
{
	if (frameLobby != nullptr)
		frameLobby->removePlayerFromBattle(battleId, id);
}


void MainWindow::updateCountUsersInTeamBattle(const QString &battleId, int redPeople, int bluePeople)
{
	if (frameLobby != nullptr)
		frameLobby->updateCountUsersInTeamBattle(battleId, redPeople, bluePeople);
}

void MainWindow::updateCountUsersInDmBattle(const QString &battleId, int people)
{
	if (frameLobby != nullptr)
		frameLobby->updateCountUsersInDmBattle(battleId, people);
}

void MainWindow::removeBattle(const QString &battleId)
{
	if (frameLobby != nullptr)
		frameLobby->removeBattle(battleId);
}



void MainWindow::on_pushButton_battles_clicked()
{
	this->panel->clickBattles();
}


void MainWindow::on_checkBox_render_off_clicked(bool checked)
{
	_renderEnable = checked == false;
	if (frameBattle != nullptr)
	{
		frameBattle->setDrawEnable(!checked);
		frameBattle->repaint();
	}
}



void MainWindow::on_toolButton_bot_clicked()
{
#ifdef TANK_BOT
	QSize size = menu_bot->sizeHint();
	QPoint pos = this->mapToGlobal(ui->toolButton_bot->pos());
	QRect geometry = ui->toolButton_bot->geometry();
	pos.setX((pos.x() - (size.width() / 2)) + (geometry.width() / 2));
	pos.setY(pos.y() + geometry.height() + 1);
	menu_bot->move(pos);
	menu_bot->exec();
#endif
}


void MainWindow::on_checkBox_bot_enable_clicked(bool checked)
{
#ifdef TANK_BOT
	if (this->bot != nullptr)
	{
		this->bot->setEnable(checked);
		this->updateCountPoints();
	}
#else
	(void)checked;
#endif
}


void MainWindow::showEvent(QShowEvent*)
{
	_hide = false;
	_renderEnable = ui->checkBox_render_off->isChecked() == false;
	if (frameBattle != nullptr)
		frameBattle->setDrawEnable(ui->checkBox_render_off->isChecked() == false);
}

void MainWindow::hideEvent(QHideEvent*)
{
	_hide = true;
	_renderEnable = false;
	if (frameBattle != nullptr)
		frameBattle->setDrawEnable(false);
}


void MainWindow::on_toolButton_clicked()
{
	show_send_package();
}

void MainWindow::on_toolButton_c_clicked()
{
	//memory_check_clear();
}


void MainWindow::on_toolButton_p_clicked()
{
	//memory_check_print();
}

void MainWindow::lobbyShowGarage(GarageModel *garageModel)
{
	showGarage(garageModel);
}

void MainWindow::registrationCaptcha(const QByteArray &data)
{
	if (frameRegistration != nullptr)
		frameRegistration->captcha(data);
}


void MainWindow::init()
{
	this->panel = Service::getPanelModel();

	/*
	Lobby *lobby = new Lobby(Service::getNetworker());
	Service::setLobby(lobby);

	QByteArray init_garage_items_json;
	File::read("init_garage_items.json", init_garage_items_json);

	lobby->parseGarageItems(init_garage_items_json, "");

	GarageModel *garageModel = lobby->garage->garageModel;

	garageModel->mountItem(nullptr, nullptr, "wasp_m0");
	garageModel->mountItem(nullptr, nullptr, "smoky_m0");
	garageModel->mountItem(nullptr, nullptr, "green_m0");
	*/
}

void MainWindow::on_pushButton_exit_clicked()
{
	//this->panel->clickExit();
}


void MainWindow::on_pushButton_garage_clicked()
{
	//this->panel->clickGarage();
}


void MainWindow::log(const QString &text)
{
	console_log.append(text);

	if (frameConsole != nullptr)
		frameConsole->add(text);
}

void MainWindow::showConsole()
{
	if (frameConsole != nullptr)
		return;

	this->frameConsole = new FrameConsole;
	this->frameConsole->setMaximumSize(QSize(16777215, 150));
	this->frameConsole->setFrameShape(QFrame::StyledPanel);
	this->frameConsole->setFrameShadow(QFrame::Raised);
	ui->gridLayout_3->addWidget(this->frameConsole, 1, 0, 1, 1);

	for (const QString &text : console_log)
		frameConsole->add(text);

	this->frameConsole->setFocus();
}

void MainWindow::hideConsole()
{
	if (frameConsole == nullptr)
		return;

	delete frameConsole;
	frameConsole = nullptr;
}

bool MainWindow::renderEnable()
{
	return _renderEnable;
}

void MainWindow::setFps(uint32_t value)
{
	if (!_hide && !ui->frame->isHidden())
	{
		ui->label_fps->setText(QString::number(value));
	}
}

void MainWindow::clearFps()
{
	ui->label_fps->clear();
}
