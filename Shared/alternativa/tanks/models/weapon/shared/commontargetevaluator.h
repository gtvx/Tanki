#ifndef COMMONTARGETEVALUATOR_H
#define COMMONTARGETEVALUATOR_H

#include "ICommonTargetEvaluator.h"

class TankData;
class ICTFModel;
class ShotData;
class IBattleField;
class IWeaponWeakeningModel;
class IModelService;

class CommonTargetEvaluator : public ICommonTargetEvaluator
{
	TankData *localTankData;
	ICTFModel *ctfModel;
	double fullDamageDistance;
	double maxAngle;

public:
	CommonTargetEvaluator(TankData *localTankData,
						  ICTFModel *ctfModel,
						  double fullDamageDistance,
						  double maxAngle);

	virtual ~CommonTargetEvaluator();

	static CommonTargetEvaluator *create(TankData *localTankData,
										 ShotData *localShotData,
										 IBattleField *battlefieldModel,
										 bool weaponWeakeningModel,
										 IModelService *modelService);

	double getTargetPriority(Body *target, double distance, double angle);

private:
	double getPriority(double distance, double angle);
};

#endif // COMMONTARGETEVALUATOR_H
