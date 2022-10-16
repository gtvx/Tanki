#include "commontargetevaluator.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/ctf/ICTFModel.h"
#include "alternativa/tanks/models/weapon/weakening/weaponweakeningmodel.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"


#define DISTANCE_WEIGHT 0.65
#define MAX_PRIORITY 1000



CommonTargetEvaluator::CommonTargetEvaluator(TankData *localTankData, ICTFModel *ctfModel, double fullDamageDistance, double maxAngle)
{
	this->localTankData = localTankData;
	this->ctfModel = ctfModel;
	this->fullDamageDistance = fullDamageDistance;
	this->maxAngle = maxAngle;
}

CommonTargetEvaluator::~CommonTargetEvaluator()
{

}

CommonTargetEvaluator* CommonTargetEvaluator::create(TankData *localTankData,
													 ShotData *localShotData,
													 IBattleField *battlefieldModel,
													 bool weaponWeakeningModel,
													 IModelService *modelService)
{
	(void)battlefieldModel;
	(void)modelService;

	double baseDistance;
	if (weaponWeakeningModel == true)
		baseDistance = WeaponWeakeningModel::getFullDamageRadius(localTankData->turret);
	else
		baseDistance = 10000.;

	double maxAngle = (localShotData->autoAimingAngleUp > localShotData->autoAimingAngleDown) ? localShotData->autoAimingAngleUp : localShotData->autoAimingAngleDown;
	return new CommonTargetEvaluator(localTankData, nullptr, baseDistance, maxAngle);

}


double CommonTargetEvaluator::getTargetPriority(Body *target, double distance, double angle)
{
	Tank *targetTank = (Tank*)target;
	if (targetTank == nullptr)
		return 0.;

	if (targetTank->tankData->health == 0)
		return 0.;

	BattleTeamType targetTeamType = targetTank->tankData->teamType;
	if (targetTeamType == BattleTeamType::NONE)
		return this->getPriority(distance, angle);

	if (targetTeamType == this->localTankData->teamType)
		return 0.;

	double priority = this->getPriority(distance, angle);
	if (!(this->ctfModel == nullptr) && (this->ctfModel->isFlagCarrier(targetTank->tankData)))
		priority += MAX_PRIORITY;

	return priority;
}

double CommonTargetEvaluator::getPriority(double distance, double angle)
{
	return MAX_PRIORITY - (((DISTANCE_WEIGHT * distance) / this->fullDamageDistance) + (((1. - DISTANCE_WEIGHT) * angle) / this->maxAngle));
}
