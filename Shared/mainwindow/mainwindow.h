#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


#include "IMain.h"
#include "projects/tanks/client/battleservice/model/battletype.h"
#include "__global.h"


class Vector3;
class Tank;
class QTreeWidgetItem;
class Damage;
class BotSettings;
class Bot;
class FrameLobby;
class FrameBattle;
class GraphicsFrameBattle;
class EnterBattle;
class Settings;
class GraphicsFrameGarage;
class GarageModel;
class PanelModel;
class FrameConsole;
class FrameRegistration;



class MainWindow : public QMainWindow, public IMain
{
	Q_OBJECT

public:
	MainWindow(const Settings *settings, QWidget *parent = nullptr);
	~MainWindow();

	void showLobby() override;
	void hideLobby() override;
	void showBattle() override;
	void hideBattle() override;
	void setNick(const QString &nick) override;
	void setRank(int rank) override;
	void setCrystal(int crystal) override;
	void clearBattles() override;
	void addBattle(const BattleClient&) override;
	void showBattleInfo(const ShowBattleInfo&) override;
	void addPlayerToBattle(const QString &battleId, const UserInfoClient &) override;
	void removePlayerFromBattle(const QString &battleId, const QString &id) override;
	void updateCountUsersInTeamBattle(const QString &battleId, int redPeople, int bluePeople) override;
	void updateCountUsersInDmBattle(const QString &battleId, int people) override;
	void removeBattle(const QString &battleId) override;
	void getSize(int *width, int *height) override;
	void initTank(Tank*) override;
	void deleteTank(Tank*) override;
	void createDamageEffect(Tank *tank, int damage, uint32_t color) override;
	bool getBotEnable() override;
	void showSuicideIndicator() override;
	void hideSuicideIndicator() override;
	void battleChatMessage(const BattleChatMessage &message) override;
	void battleEntrance(EnterBattle *enterBattle);
	void socketConnected() override;
	void socketDisconnected() override;
	void noConnection() override;
	void wrongPassword() override;
	void captcha() override;
	void ban() override;
	void log(const QString&) override;
	bool renderEnable() override;
	void setFps(uint32_t value) override;
	void clearFps() override;
	void showGarage(GarageModel *garageModel);
	void lobbyShowGarage(GarageModel *garageModel) override;
	void registrationCaptcha(const QByteArray &data) override;

	void init();


	void hideGarage();


	void showRegistration(Network *network) override;
	void hideRegistration() override;
	void registrationNotExist() override;
	void registrationIncorrect() override;
	void registrationNicknameExist() override;
	void registrationNicknameNotMatchPattern() override;

private slots:
	void on_pushButton_battles_clicked();
	void on_checkBox_render_off_clicked(bool checked);
	void on_toolButton_bot_clicked();
	void on_checkBox_bot_enable_clicked(bool checked);
	void on_toolButton_clicked();
	void on_toolButton_c_clicked();
	void on_toolButton_p_clicked();
	void on_pushButton_exit_clicked();
	void on_pushButton_garage_clicked();

private:
	Ui::MainWindow *ui;
	void paintEvent(QPaintEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void draw() override;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override;


	int rank;

	QMenu *menu_bot;
	Bot *bot;
	BotSettings *botSettings;
	FrameLobby *frameLobby;
	GraphicsFrameGarage *frameGarage;

#ifdef GRAPHICS
	GraphicsFrameBattle *frameBattle;
#else
	FrameBattle *frameBattle;
#endif

	bool _renderEnable;
	bool _hide;
	EnterBattle *_enterBattle;
	Settings const *settings;
	PanelModel *panel;
	FrameConsole *frameConsole;
	FrameRegistration *frameRegistration;

	QStringList console_log;
	void updateCountPoints();
	void showConsole();
	void hideConsole();
};

#endif // MAINWINDOW_H
