#ifndef TERMINATORTARGETSYSTEM_H
#define TERMINATORTARGETSYSTEM_H

#include <QHash>

class ICTFModel;
class ICollisionDetector;
class TankData;
class Vector3;
class TerminatorShotResult;
class Array;
class TerminatorMultybodyCollisionPredicate;
class Body;
class ListVector3;


class TerminatorTargetSystem
{
	ICollisionDetector *collisionDetector;
	TerminatorMultybodyCollisionPredicate *multybodyPredicate;
	ICTFModel *ctfModel;
	double upAngleStep;
	double downAngleStep;
	double weakeningCoeff;
	double maxDirectionPriority;
	double bestEffectivity;
	double maxAngle;
	int downRaysNum;
	int upRaysNum;
	int bestDirectionIndex;

	ListVector3 *currHitPoints;
	QVector<TankData*> currTargets;


public:
	TerminatorTargetSystem();
	~TerminatorTargetSystem();


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
					TerminatorShotResult *shotResult);

	void checkAllDirections(TankData *shooterData,
							const Vector3 *barrelOrigin,
							const Vector3 *baseDir,
							const Vector3 *rotationAxis,
							QHash<Body*, TankData*> *tanks,
							TerminatorShotResult *shotResult);

	void checkDirection(const Vector3 *barrelOrigin,
						const Vector3 *barrelDir,
						double angle,
						int dirIndex,
						TankData *shooterData,
						QHash<Body*, TankData*> *tanks,
						TerminatorShotResult *shotResult);

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
								TerminatorShotResult *shotResult);

	double getDirectionPriority(double angle, double distance);
};

#endif // TERMINATORTARGETSYSTEM_H
