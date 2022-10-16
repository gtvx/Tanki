#include "bot_flamethrower.h"
#include "tanks.h"
#include "tank_control.h"
#include "bot_utils.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/physics/bodystate.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "alternativa/tanks/models/weapon/common/weaponcommonmodel.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/weapon/flamethrower/flamethrowermodel.h"
#include "battlecontroller.h"
#include "alternativa/math/matrix4.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/weapon/shared/conicareatargetsystembot.h"
#include <math.h>
#include "service.h"
#include "_global.h"
#include <QVector>
//#include <QDebug>


//muzzleLocalPos;
//x 0
//y 322.8154296875
//z 42.21024322509765625





static bool find_tank(QVector<Body*> *targetIds, Tank *tank)
{
	for (int i = 0; i < targetIds->count(); i++)
	{
		Tank *item_tank = (Tank*)targetIds->at(i);
		if (item_tank == tank) {
			return true;
		}
	}

	return false;
}

static Tank* find_tank_enemy(QVector<Body*> *targetIds, Tank *my_tank)
{
	for (int i = 0; i < targetIds->count(); i++)
	{
		Tank *tank = (Tank*)targetIds->at(i);
		if (TANKS::is_tank_enemy(tank, my_tank))
			return tank;
	}

	return nullptr;
}


void BotFlamethrower::startFire()
{
	if (local_tank->tankData->spawnState == TankSpawnState::ACTIVE)
	{
		if (weapon->indicatorValue > 0.3 || flamethrowerModel->active == true)
		{
			weapon->triggerKeyDown = true;
			weapon->triggerKeyUp = false;
			bot_shoot = true;
		}
		else
		{
			stopFire();
		}
	}
	else
	{
		stopFire();
	}
}

void BotFlamethrower::stopFire()
{
	if (weapon->triggerKeyDown == true && bot_shoot == true)
		weapon->triggerKeyUp = true;
	bot_shoot = false;
}


bool BotFlamethrower::init()
{
	battleController = Service::getBattleController();
	if (battleController == nullptr)
		return false;

	flamethrowerModel = battleController->flamethrowerModel;
	if (flamethrowerModel == nullptr)
		return false;

	battlefield = Service::getBattlefieldModel();
	if (battlefield == nullptr)
		return false;

	local_tank = TANKS::get_my_tank();
	if (local_tank == nullptr)
		return false;

	weapon = (WeaponCommonModel*)local_tank->tankData->weapon;

	tank_current = nullptr;
	body_turn = BodyTurn::NO;
	tanks_check = false;
	bot_shoot = false;

	WeaponCommonData *weaponCommonData = flamethrowerModel->localWeaponCommonData;
	if (weaponCommonData->muzzleLength() != 1)
		return false;

	const Vector3 *muzzle = weaponCommonData->getMuzzle(0);

	barrelLength = muzzle->y;

	localBarrelOrigin.x = muzzle->x;
	localBarrelOrigin.y = 0.0;
	localBarrelOrigin.z = muzzle->z;


	return true;
}


void BotFlamethrower::processing(uint32_t time)
{
	_function_name("BotFlamethrower::processing");

	(void)time;

	if (local_tank->tankData->spawnState == TankSpawnState::SUICIDE) {
		stopFire();
		return;
	}

	if (tank_current != nullptr && tank_current->tankData->spawnState != TankSpawnState::ACTIVE)
		tank_current = nullptr;


	TankSkin *skin = local_tank->skin;
	Mesh *hullMesh = skin->hullMesh;
	Matrix4 hullMatrix;
	hullMatrix.setMatrix(&hullMesh->position, &hullMesh->rotation);

	const Vector3 &local_pos = local_tank->state.pos;
	int32_t local_x_int = local_pos.x;
	int32_t local_y_int = local_pos.y;

	if (tank_current == nullptr)
	{
		stopFire();
	}


	bool _distance_init = false;


	tanks_check = true;

	for (;;)
	{
		if (tank_current == nullptr)
		{
			if (_distance_init == false)
			{
				_distance_init = true;
				distanceInit();
			}

			TANKS::StructTank *current = this->getNearTank(2100);
			if (current == nullptr) {
				clearFocus();
			} else {
				setTankCurrent(current);
			}
		}


		if (tank_current == nullptr) {
			body_turn = BodyTurn::NO;
			if (tankControl->is_turret() == true)
			{
				tankControl->turret_stop();
				//tankControl->apply();
			}
			stopFire();
			return;
		}

		if (tank_current->tankData->spawnState != TankSpawnState::ACTIVE) {
			tank_current = nullptr;
			continue;
		}

		const Vector3 &target_pos = tank_current->state.pos;
		int32_t target_x_int = target_pos.x;
		int32_t target_y_int = target_pos.y;


		double v1x, v1y;
		BotUtils::cords_to_dir(local_x_int, local_y_int, target_x_int, target_y_int, &v1x, &v1y);


		Matrix4 orientation;
		orientation.d = 0.;
		orientation.h = 0.;
		orientation.l = 0.;

		Vector3 v2;
		local_tank->state.orientation.toMatrix4(&orientation);
		orientation.transformVector(&Vector3::Y_AXIS, &v2);
		v2.normalize();

		const double hull_direction = atan2(v2.y, v2.x);
		const double direction_center = BotUtils::direction_normalize(atan2(v1y, v1x) - hull_direction);


		double dir_step = BotUtils::dir_step(local_x_int, local_y_int, target_x_int, target_y_int, direction_center, hull_direction);



		ConicAreaTargetSystemBot *targetSystem = flamethrowerModel->targetSystemBot;


		double direction_need;

		bool ok = false;


		Vector3 xAxis;
		Vector3 gunGlobalDir;
		Vector3 firingOrigin;


		{	
			BotUtils::calculate_gun_params(skin, &hullMatrix, &this->localBarrelOrigin, direction_center, &firingOrigin, &xAxis, &gunGlobalDir);
			targetIds->clear();
			targetSystem->getTargets(local_tank, barrelLength, 0.3, &firingOrigin, &gunGlobalDir, &xAxis, targetIds);


			if (find_tank(targetIds, tank_current))
			{
				direction_need = direction_center;
				ok = true;
			}
		}

		if (ok == false)
		{
			double direction_left = direction_center;
			double direction_right = direction_center;

			for (int i = 0; i < 5; i++)
			{
				{
					direction_left = BotUtils::direction_normalize(direction_left - dir_step);
					BotUtils::calculate_gun_params(skin, &hullMatrix, &this->localBarrelOrigin, direction_left, &firingOrigin, &xAxis, &gunGlobalDir);
					targetIds->clear();
					targetSystem->getTargets(local_tank, barrelLength, 0.3, &firingOrigin, &gunGlobalDir, &xAxis, targetIds);

					if (find_tank(targetIds, tank_current))
					{
						direction_need = direction_left;
						ok = true;
						break;
					}
				}

				{
					direction_right = BotUtils::direction_normalize(direction_right + dir_step);
					BotUtils::calculate_gun_params(skin, &hullMatrix, &this->localBarrelOrigin, direction_right, &firingOrigin, &xAxis, &gunGlobalDir);

					targetIds->clear();
					targetSystem->getTargets(local_tank, barrelLength, 0.3, &firingOrigin, &gunGlobalDir, &xAxis, targetIds);

					if (find_tank(targetIds, tank_current))
					{
						direction_need = direction_right;
						ok = true;
						break;
					}
				}
			}
		}

		if (ok == true)
		{
			{
				bool stop_fire = true;

				BotUtils::calculate_gun_params(skin, &hullMatrix, &this->localBarrelOrigin, skin->turretDirection, &firingOrigin, &xAxis, &gunGlobalDir);

				targetIds->clear();
				targetSystem->getTargets(local_tank, barrelLength, 0.3, &firingOrigin, &gunGlobalDir, &xAxis, targetIds);


				if (find_tank_enemy(targetIds, local_tank))
				{
					if (tank_current->tankData->spawnState == TankSpawnState::ACTIVE &&
						local_tank->tankData->spawnState == TankSpawnState::ACTIVE)
					{
						BotUtils::update_turret_transform(skin, &hullMatrix);
						startFire();
						stop_fire = false;
					}
				}
				if (stop_fire == true)
				{
					stopFire();
				}
			}



			double v = BotUtils::difference(direction_need, skin->turretDirection);
			double s = abs(v);
			if (s  > 0.02)
			{
				bool c = s > 0.5;
				if (v > 0.)
				{
					tankControl->turret_left();
					body_turn = c == true ? BodyTurn::LEFT : BodyTurn::NO;
				}
				else
				{
					tankControl->turret_right();
					body_turn = c == true ? BodyTurn::RIGHT : BodyTurn::NO;
				}

				//tank_control_apply();
			}
			else
			{
				body_turn = BodyTurn::NO;
				tankControl->turret_stop();
				//tankControl->apply();
				skin->turretDirection = direction_need;
			}

			break;
		}
		else
		{
			tank_current_null();
		}
	}
}

void BotFlamethrower::tank_current_null()
{
	if (tank_current == nullptr)
		return;

	tank_current = nullptr;
	body_turn = BodyTurn::NO;
	if (tankControl->is_turret() == true)
	{
		tankControl->turret_stop();
	}
	stopFire();
}

BotFlamethrower::BotFlamethrower(TankControl *tankControl)
{
	this->tankControl = tankControl;
	targetIds = new QVector<Body*>;
}

BotFlamethrower::~BotFlamethrower()
{
	delete targetIds;
}

void BotFlamethrower::deleteTank(Tank *tank)
{
	if (tank_current == tank)
		tank_current_null();
}
