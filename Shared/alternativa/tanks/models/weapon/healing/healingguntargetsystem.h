#ifndef HEALINGGUNTARGETSYSTEM_H
#define HEALINGGUNTARGETSYSTEM_H

#include <QSet>

class TankData;
class HealingGunData;
class Vector3;
class TanksCollisionDetector;
class Dictionary;
class ICollisionDetector;
class HealingGunPredicate;
enum class BattleTeamType;
class Body;

class HealingGunTargetSystem
{
	HealingGunPredicate *predicate;
	QSet<Body*> checkedTargets;
	TankData *bestTarget;
	BattleTeamType shooterTeamType;
	double maxTargetPriority;


public:
	HealingGunTargetSystem();
	~HealingGunTargetSystem();


	TankData* getTarget(TankData *healerData,
						HealingGunData *gunData,
						double barrelLength,
						Vector3 *barrelOrigin,
						Vector3 *gunDir,
						Vector3 *xAxis,
						int numRays,
						double numSteps,
						TanksCollisionDetector *collisionDetector,
						QHash<Body*, TankData*> &tankDatas);

	bool validateTarget(TankData *healerData,
						TankData *targetData,
						HealingGunData *gunData,
						Vector3 *gunDir,
						Vector3 *barrelOrigin,
						double barrelLength,
						TanksCollisionDetector *collisionDetector);

private:
	void checkSector(HealingGunData *gunData,
					 double lockingRangeSqr,
					 const Vector3 *rayOrigin,
					 const Vector3 *gunDir,
					 const Vector3 *xAxis,
					 int raysNum,
					 double angleStep,
					 ICollisionDetector *collisionDetector,
					 QHash<Body*, TankData*> &tanks,
					 int step);

	void checkTarget(Body *targetBody,
					 QHash<Body*, TankData*> &tanks,
					 HealingGunData *gunData,
					 double lockingRangeSqr,
					 const Vector3 *rayOrigin,
					 const Vector3 *gunDir,
					 ICollisionDetector *collisionDetector);

	double getBaseTargetPriority(TankData *targetData);
};

#endif // HEALINGGUNTARGETSYSTEM_H
