#ifndef RAILGUNTARGETSYSTEM_H
#define RAILGUNTARGETSYSTEM_H

#include <QHash>

class ICTFModel;
class ICollisionDetector;
class TankData;
class Vector3;
class Dictionary;
class RailgunShotResult;
class Array;
class Body;
class RailgunMultybodyCollisionPredicate;
class ListVector3;


class RailgunTargetSystem
{
	ICollisionDetector *collisionDetector;
	RailgunMultybodyCollisionPredicate *multybodyPredicate;
	QVector<TankData*> currTargets;
	ListVector3 *currHitPoints;

	ICTFModel *ctfModel;

	double upAngleStep;
	double weakeningCoeff;
	double downAngleStep;
	double maxDirectionPriority;
	double bestEffectivity;
	double maxAngle;

	int bestDirectionIndex;
	int upRaysNum;
	int downRaysNum;

public:
	RailgunTargetSystem();
	~RailgunTargetSystem();


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
					RailgunShotResult *shotResult);


private:
	void checkAllDirections(TankData *shooterData,
							const Vector3 *barrelOrigin,
							const Vector3 *baseDir,
							const Vector3 *rotationAxis,
							QHash<Body*, TankData*> *tanks,
							RailgunShotResult *shotResult);

	void checkDirection(const Vector3 *barrelOrigin,
						const Vector3 *barrelDir,
						double angle,
						int dirIndex,
						TankData *shooterData,
						QHash<Body*, TankData*> *tanks,
						RailgunShotResult *shotResult);

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
								RailgunShotResult *shotResult);


	double getDirectionPriority(double angle, double distance);
};

#endif // RAILGUNTARGETSYSTEM_H
