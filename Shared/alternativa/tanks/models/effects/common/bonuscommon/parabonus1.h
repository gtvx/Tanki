#ifndef PARABONUS1_H
#define PARABONUS1_H

#include <QString>
#include "alternativa/tanks/bonuses/IBonus.h"
#include "alternativa/physics/collision/ICollisionPredicate.h"
#include "alternativa/tanks/bonuses/BonusState.h"
#include "alternativa/physics/rigid/skinnedbody3d.h"
#include "__bonus.h"


class IBonusListener;
class ParaBonusPool;
class Mesh;
class FallController;
class LandingController;
class Parachute1;
class Cords1;
class OmniLight;
class BonusTrigger;
class HidableObject3DWrapper;
class BonusCommonData;
class Material;
class BonusMesh;
class BattlefieldModel;
class CollisionBox;
class MaxDistanceConstraint;
class HidableObject3DWrapper;


class ParaBonus1 :
		public SkinnedBody3D,
		public IBonus,
		public ICollisionPredicate
{
	QString _bonusId;

	int timeToLive;
	int currBlinkInterval;
	int visibilitySwitchTime;
	int takenAnimationTime;
	int parachuteTimeLeft;
	int additiveValue;
	BonusState bonusState;
	PhysicsScene *physicsScene;
	HidableObject3DWrapper *hidabbleWrapper;
	std::shared_ptr<MaxDistanceConstraint> cordsConstaints[4];
	CollisionBox *collisionBox;

	enum class PHYSICS_STATE
	{
		NONE,
		FULL,
		PARABOX,
		BOX,
	};

	PHYSICS_STATE physicsState;

	IBonusListener *bonusListener;
	double alphaSpeed;
	BattlefieldModel *battlefieldModel;

public:
	static const T *TYPE;
	std::shared_ptr<Parachute1> parachute;
	std::shared_ptr<Cords1> cordsMesh;
	//OmniLight *light;
	std::shared_ptr<Mesh> _boxMesh;


	ParaBonus1(
			const T *type,
			ParaBonusPool *pool,
			std::shared_ptr<Mesh> &boxMesh,
			std::shared_ptr<Mesh> &parachuteMesh,
			std::shared_ptr<Mesh> &parachuteInnerMesh,
			Material *cordMaterial);
	~ParaBonus1();

	static ParaBonus1* create(BonusCommonData *bonusData);

	CollisionBox* getCollisionBox();
	Mesh* getMesh();

private:
	static bool isFlatSurface(Vector3 *groundNormal);

public:
	void init(const QString &bonusId, int timeToLive, bool isFalling);

private:
	void getGroundPointAndNormal(const Vector3 *spawnPosition, Vector3 *point, Vector3 *normal);

public:
	QString getBonusId();
	bool isFalling();
	void readBonusPosition(Vector3 *result);
	//void setBonusPosition(double x, double y, double z);
	//void setRestingState(double x, double y, double z);
	void setTakenState();
	void setRemovedState();
	void attach(const Vector3 *pos, PhysicsScene *rigidWorld, Scene3DContainer *container, IBonusListener *listener) override;
	void beforePhysicsStep(double dt);

public:
	void onLandingComplete();
private:
	double getStartingAngleZ();
	double calculateFallTime(const Vector3 *spawnPosition, const Vector3 *groundTouchPoint);

public:
	bool update(int time, int millis, double interpolationTime);
	void destroy();
	bool considerCollision(CollisionPrimitive *primitive);
	void onTriggerActivated();
	void onStaticCollision();

private:
	void detachParachute();
	void startParachuteDissolving();
	void onTankCollision();
	void createParachuteAndCords(std::shared_ptr<Mesh> &parachuteMesh, std::shared_ptr<Mesh> &parachuteInnerMesh, Material *cordsMaterial);
	void removeParachuteGraphics();
	void removeParachutePhysics();
	void playWarningAnimation(int time, double deltaMsec);
	void playTakenAnimation(int millis);
	void updateSkin(double t);
	void removeCordConstraints();
};


#endif // PARABONUS1_H
