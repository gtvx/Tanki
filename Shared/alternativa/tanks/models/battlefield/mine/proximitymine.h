#ifndef PROXIMITYMINE_H
#define PROXIMITYMINE_H

#include <QString>
#include <memory>
#include "alternativa/math/vector3.h"

class CollisionSphere;
enum class BattleTeamType;
class Mesh;
class MineModelData;
class TankData;
class Material;
class Scene3DContainer;


class ProximityMine
{
	BattleTeamType teamType;
	std::shared_ptr<Mesh> mesh;
	bool armed;
	MineModelData *mineModelData;
	int flashBaseTime;
	int flashState;
	//blinker:Blinker = new Blinker(INITIAL_BLINK_INTERVAL, MIN_BLINK_INTERVAL, BLINK_INTERVAL_DECREMENT, ALPHA_MIN, 1, BLINK_SPEED_COEFF);

public:
	QString id;
	QString ownerId;
	bool hitCommandSent;

	CollisionSphere *collisionPrimitive;
	Vector3 position;
	Vector3 normal;
	ProximityMine *next;
	ProximityMine *prev;

	ProximityMine(std::shared_ptr<Mesh> &referenceMesh);
	~ProximityMine();

	static ProximityMine* create(const QString &id, const QString &ownerId, double proximityRadius, std::shared_ptr<Mesh> &referenceMesh, Material *material, BattleTeamType teamType, MineModelData *mineModelData);

	void dispose();
	void arm();
	void setPosition(const Vector3 *pos, const Vector3 *normal);
	void addToContainer(Scene3DContainer *container);
	bool canExplode(TankData *tankData);
	void update(int now, int deltaMillis, TankData *localUserData);

private:
	void init(const QString &id, const QString &ownerId, double proximityRadius, Material *material, BattleTeamType teamType, MineModelData *mineModelData);
	void setProximityRadius(double value);
	void updateVisibility(TankData *localUserData);
	void updateFlash(int now);
};

#endif // PROXIMITYMINE_H
