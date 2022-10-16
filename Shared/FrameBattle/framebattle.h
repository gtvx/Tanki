#ifndef FRAMEBATTLE_H
#define FRAMEBATTLE_H

#include <QFrame>
#include <QTimer>
#include "graphics.h"

class Network;
class BattleChatMessage;
class Damage;
class Tank;
class Settings;
class Bot;
class BotSettings;
class BattlefieldModel;


namespace Ui {
	class FrameBattle;
}

class FrameBattle : public QFrame
{
	Q_OBJECT
	Network *network;
public:
	explicit FrameBattle(Network *network);
	~FrameBattle();

	void battleChatMessage(const BattleChatMessage &message);
	void enter();

	void createDamageEffect(Tank *tank, int damage, uint32_t color);
	void showSuicideIndicator();
	void hideSuicideIndicator();

	void setDrawEnable(bool enable);

	void setSettings(const Settings *settings);


	void initTank(Tank *tank);
	void setBot(Bot *bot);

	void draw();

	void getSize(int *width, int *height);

	bool init();

private slots:
	void on_lineEdit_chat_enter_returnPressed();

private:
	Ui::FrameBattle *ui;
	bool lock_hide_chat;
	QTimer timer_chat;

	void showChat();
	void hideChat();
	void showAllChat();
	void hideAllChat();
	bool isFocusChat();
	void sendMessage(const QString &msg, bool team);
	void slot_time_out();
	void paintEvent(QPaintEvent *event);

	bool draw_enable;
	int suicideIndicatorAnimation;
	bool suicideIndicatorAnimationUp;
	bool suicideIndicator;

	Graphics graphics;
	QVector<Damage*> damages;

	Settings const *settings;

	Bot *bot;
	BotSettings *botSettings;

	BattlefieldModel *battlefieldModel;

	void resizeEvent(QResizeEvent *event) override;
};

#endif // FRAMEBATTLE_H
