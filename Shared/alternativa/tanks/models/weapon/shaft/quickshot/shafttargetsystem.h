#ifndef SHAFTTARGETSYSTEM_H
#define SHAFTTARGETSYSTEM_H

#include <QHash>

class ICollisionDetector;
class ICTFModel;
class TankData;
class Vector3;
class Dictionary;
class ShaftShotResult;
class Array;
class Body;
class ShaftMultybodyCollisionPredicate;
class ListVector3;


class ShaftTargetSystem
{
	int upRaysNum;
	int downRaysNum;
	int bestDirectionIndex;
	double upAngleStep;
	double downAngleStep;
	double weakeningCoeff;
	double maxDirectionPriority;
	double bestEffectivity;
	double maxAngle;
	ICollisionDetector *collisionDetector;
	ShaftMultybodyCollisionPredicate *multybodyPredicate;
	QVector<TankData*> currTargets;
	ListVector3 *currHitPoints;
	ICTFModel *ctfModel;

public:
	ShaftTargetSystem();
	~ShaftTargetSystem();


	void setParams(ICollisionDetector *collisionDetector,
				   double upAngle,
				   int upRaysNum,
				   double downAngle,
				   int downRaysNum,
				   double weakeningCoeff,
				   ICTFModel *ctfModel);

	void getTargets(TankData *shooterData,
					const Vector3 *barrelOrigin,
					const Vector3 *baseDir,
					const Vector3 *rotationAxis,
					QHash<Body*, TankData*> *tanks,
					ShaftShotResult *shotResult);

private:
	void checkAllDirections(TankData *shooterData,
							const Vector3 *barrelOrigin,
							const Vector3 *baseDir,
							const Vector3 *rotationAxis,
							QHash<Body*, TankData*> *tanks,
							ShaftShotResult *shotResult);

	void checkDirection(const Vector3 *barrelOrigin,
						const Vector3 *barrelDir,
						double angle,
						int dirIndex,
						TankData *shooterData,
						QHash<Body*, TankData*> *tanks,
						ShaftShotResult *shotResult);

	double evaluateDirection(const Vector3 *barrelOrigin,
							 const Vector3 *barrelDir,
							 TankData *shooterData,
							 QHash<Body*, TankData*> *tanks,
							 QVector<TankData*> *targets,
							 ListVector3 *hitPoints);

	void collectTargetsAlongRay(TankData *shooterData,
								const Vector3 *barrelOrigin,
								const Vector3 *barrelDir,
								QHash<Body*, TankData*> *tanks,
								ShaftShotResult *shotResult);

	double getDirectionPriority(double angle, double distance);

};

#endif // SHAFTTARGETSYSTEM_H
