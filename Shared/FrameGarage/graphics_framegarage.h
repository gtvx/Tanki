#ifndef FRAMEGARAGE_H
#define FRAMEGARAGE_H

#include <QFrame>
#include <QTimer>

class Network;
class BattleChatMessage;
class Damage;
class Tank;
class Settings;
class Bot;
class BotSettings;
class DirectXView;
class GarageModel;
class Stage;

namespace Ui {
	class GraphicsFrameGarage;
}

class GraphicsFrameGarage : public QFrame
{
	Q_OBJECT
	Network *network;
public:
	explicit GraphicsFrameGarage(Network *network);
	~GraphicsFrameGarage();

	void setDrawEnable(bool enable);

	void setSettings(const Settings *settings);

	void draw();

	void getSize(int *width, int *height);

	bool init(GarageModel *garageModel);

private:
	Ui::GraphicsFrameGarage *ui;

	bool draw_enable;

	Settings const *settings;

	Stage *stage;

	GarageModel *garageModel;

	void resizeEvent(QResizeEvent *event) override;

	QTimer timer;
};

#endif // FRAMEGARAGE_H
