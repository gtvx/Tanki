#ifndef FRAMEBATTLE_H
#define FRAMEBATTLE_H

#include <QFrame>

class Network;
class BattleChatMessage;
class Damage;
class Tank;
class Settings;
class Bot;
class BotSettings;
class Stage;
class BattlefieldModel;


namespace Ui {
	class GraphicsFrameBattle;
}

class GraphicsFrameBattle : public QFrame
{
	Q_OBJECT
	Network *network;
public:
	explicit GraphicsFrameBattle(Network *network);
	~GraphicsFrameBattle();

	void setDrawEnable(bool enable);
	void setSettings(const Settings *settings);
	void initTank(Tank *tank);
	void setBot(Bot *bot);
	void draw();
	void getSize(int *width, int *height);
	bool init();


private:
	Ui::GraphicsFrameBattle *ui;

	bool draw_enable;

	Settings const *settings;

	Stage *stage;

	BattlefieldModel *battlefieldModel;

	void resizeEvent(QResizeEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
};

#endif // FRAMEBATTLE_H
