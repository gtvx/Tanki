#ifndef BOT_H
#define BOT_H

#include <stdint.h>

namespace TANKS
{
	struct StructTank;
}

class BotPrivate;
class Tank;
class BotSettings;
class MapPoints;
class TanksCollisionDetector;
class TankModel;
class MapListPath;


class Bot
{
public:
	Bot(TankModel *tankModel, TANKS::StructTank *structTank, BotSettings *botSettings, TanksCollisionDetector *collisionDetector);
	~Bot();
	void run(uint32_t time);
	void deleteTank(Tank*);
	MapPoints* getMapPoints();

	MapListPath *getMapListPath();
	void spawnTank(uint32_t time);

	BotSettings* getBotSettings();

	void savePoints();
	void loadPoints();

	void setEnable(bool status);	
	void updateCommands();

private:
	BotPrivate *d;
};

#endif // BOT_H
