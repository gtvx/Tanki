#ifndef TANK_H
#define TANK_H

#include "alternativa/physics/rigid/body3d.h"
#include "alternativa/math/vector3.h"
#include "alternativa/math/quaternion.h"
#include "SuspensionData.h"
#include <stdint.h>

class TankSkin;
class TankData;
class Track;
class CollisionBox;
class Vector3;
class Quaternion;
class IValueSmoother;
class ValueSmoother;
class TurretController;
class HullTransformUpdater;
class Vector3D;
class Mesh;
class LocalHullTransformUpdater;
class Bot;
class BotSettings;
class CalculateTrackSpeed;
class UserTitle;


namespace TANKS
{
	class StructTank;
}


class Tank : public Body3D
{
	bool leftBrake;
	bool rightBrake;

	LocalHullTransformUpdater *lockedHullTransformUpdater;
	HullTransformUpdater *hullTransformUpdater;
	HullTransformUpdater *notLockedHullUpdater;
	TurretController *turretController;
	Vector3 dimensions;
	IValueSmoother *maxSpeedSmoother;
	ValueSmoother *maxTurnSpeedSmoother;
	ValueSmoother *maxTurretTurnSpeedSmoother;
	Scene3DContainer *container;
	Scene3DContainer *titleContainer;
	UserTitle *_title;

	CalculateTrackSpeed *trackSpeed;
	double shpereRadius;
	bool deleted;
	bool local;
	bool animatedTracks;

public:
	Tank(const T *type,
		 TankSkin *skin,
		 double mass,
		 double maxSpeed,
		 double maxTurnSpeed,
		 double turretMaxTurnSpeed,
		 double turretTurnAcceleration,
		 bool isLocal,
		 Scene3DContainer *titleContainer,
		 double rayLength = 50,
		 double rayOptimalLength = 25,
		 double dampingCoeff = 1000,
		 double friction = 0.1,
		 double rays = 5);
	virtual ~Tank() override;

	static const T *TYPE;

	double maxSpeed;
	double maxTurnSpeed;
	double maxTurretTurnSpeed;
	double turretTurnAcceleration;
	double turretTurnSpeed;
	double mass;
	double skinZCorrection;
	double leftThrottle;
	double rightThrottle;

	SuspensionData suspensionData;
	CollisionBox *mainCollisionBox;
	CollisionBox *collisionBox_2;
	Track *leftTrack;
	Track *rightTrack;
	TANKS::StructTank *structTank;
	TankSkin *skin;
	TankData *tankData;
	BotSettings *botSettings;
	Bot *bot;
	Vector3 interpolatedPosition;
	Quaternion interpolatedOrientation;
	Vector3 skinCenterOffset;
	Vector3 visibilityPoints[6];
	int movedir;
	int turndir;
	int username_width;

	bool new_contact_normal;
	bool normal_contact_new;


	bool isLocal() const
	{
		return local;
	}

	UserTitle* title() { return _title; }

	double getBoundSphereRadius() { return this->shpereRadius; }

	TurretController* getTurretController();
	void setTurretController(TurretController *controller);
	void setHullTransformUpdater(HullTransformUpdater *hull);
	void setMaxSpeed(double value, bool immediate);
	void setMaxTurnSpeed(double value, bool immediate);
	void setMaxTurretTurnSpeed(double value, bool immediate);
	void getPhysicsState(Vector3 *pos, Vector3 *orient, Vector3 *linVel, Vector3 *angVel);
	int getCollisionGroup();
	void setCollisionGroup(int value);
	void setTracksCollisionGroup(int value);
	void setBrakes(bool lb, bool rb);
	void setThrottle(double throttleLeft, double throttleRight);
	void setPhysicsState(const Vector3 *pos, const Vector3 *orient, const Vector3 *linVel, const Vector3 *angVel);
	int getTurretDirSign();
	void stopTurret();
	bool rotateTurret(double timeFactor, bool stopAtZero);
	void updateSkin(double t);
	void addToContainer(Scene3DContainer *container) override;
	void removeFromContainer() override;

	void beforePhysicsStep(double dt);
	void lockTransformUpdate();
	void unlockTransformUpdate();
	double getTurretDir();
	void setTurretDir(double value);
	void destroy() override;
	void spawn();
	Bot* initBot();
	void updateBot(uint32_t time);	
	void setAnimationTracks(bool value);

private:
	void interpolatePhysicsState(double t);
	void calculateSkinCenterOffset(Vector3 *param1);
	void calculateSizeFromMesh(Mesh *mesh, Vector3 *out);
	void setBoundSphereRadius(Vector3 *radius, double p1);
	void setMassAndDimensions(double mass, Vector3 *dimensions);
	void setupCollisionPrimitives();
	void createTracks(int raysNum, double trackLength, double widthBetween);
	double getVelocity();
};

#endif // TANK_H


