#ifndef BATTLEFIELDDATA_H
#define BATTLEFIELDDATA_H

#include <stdint.h>
#include <qglobal.h>
#include <memory>

class ClientObject;
class TanksCollisionDetector;
class PhysicsScene;
class SkyBox;
class DisplayObjectContainer;
class BattleView3D;
class Tank;
class TankData;
class ClientObjectUser;
class ISpecialEffect;
class IBonus;


class BattlefieldData
{
public:
	ClientObject *bfObject;
	ClientObjectUser *localUser;
	//DisplayObjectContainer *guiContainer;
	BattleView3D *viewport;
	QHash<Tank*, TankData*> *tanks;
	QHash<TankData*, bool> *activeTanks;
	QHash<ISpecialEffect*, bool> *graphicEffects;
	QHash<QString, IBonus*> *bonuses;

	//Dictionary *graphicEffects = new Dictionary();
	//Dictionary *bonuses = new Dictionary();
	//Sound *bonusTakenSound;
	//Sound *battleFinishSound;
	//Sound *ambientSound;
	//SoundChannel *ambientChannel;
	//Sound *killSound;

	TanksCollisionDetector *collisionDetector;
	PhysicsScene *physicsScene;
	uint32_t physTime;
	uint32_t time;
	std::shared_ptr<SkyBox> skybox;
	uint32_t respawnInvulnerabilityPeriod;
	uint32_t idleKickPeriod;
};

#endif // BATTLEFIELDDATA_H
