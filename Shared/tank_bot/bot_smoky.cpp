#include "bot_smoky.h"
#include "tanks.h"
#include "tank_control.h"
#include "alternativa/math/matrix4.h"
#include "alternativa/math/quaternion.h"
#include "alternativa/tanks/models/weapon/gun/smokymodel.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "battlecontroller.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/tanks/models/weapon/common/weaponcommonmodel.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/weapon/shared/commontargetsystembot.h"
#include "alternativa/tanks/models/weapon/common/HitInfo.h"
#include "bot_utils.h"
#include "_global.h"
#include "service.h"
#include "gametime.h"
#include <math.h>
#include <QVector>
#include <QDebug>






static void shoot(SmokyModel *smokyModel, uint32_t current_time)
{
	smokyModel->_triggerPressed = true;
	smokyModel->update(current_time, 0);
	smokyModel->_triggerPressed = false;
}


static void shoot(SmokyModel *smokyModel, uint32_t current_time, TankSkin *skin, const Matrix4 *hullMatrix)
{
	BotUtils::update_turret_transform(skin, hullMatrix);
	shoot(smokyModel, current_time);
}



BotSmoky::BotSmoky(TankControl *tankControl)
{
	this->tankControl = tankControl;
}

BotSmoky::~BotSmoky()
{

}

bool BotSmoky::init()
{
	battleController = Service::getBattleController();
	if (battleController == nullptr)
		return false;

	smokyModel = battleController->smokyModel;
	if (smokyModel == nullptr)
		return false;

	battlefield = Service::getBattlefieldModel();
	if (battlefield == nullptr)
		return false;

	local_tank = TANKS::get_my_tank();
	if (local_tank == nullptr)
		return false;

	tank_current = nullptr;
	struct_tank_current = nullptr;

	body_turn = BodyTurn::NO;
	tanks_check = false;

	WeaponCommonData *weaponCommonData = smokyModel->localWeaponCommonData;

	if (weaponCommonData->muzzleLength() != 1)
		return false;


	const Vector3 *muzzle = weaponCommonData->getMuzzle(0);
	//x 0
	//y 281.7127685546875
	//z 41.590732574462890625

	localBarrelOrigin.x = muzzle->x;
	localBarrelOrigin.y = 0.0;
	localBarrelOrigin.z = muzzle->z;


	return true;
}


void BotSmoky::processing(uint32_t time)
{
	_function_name("BotSmoky::processing");
	//list_clear();


	if (local_tank->tankData->spawnState == TankSpawnState::SUICIDE)
		return;

	//__Camera3D *camera = (__Camera3D*)battlefield->bfData->viewport->camera;

	static double direction_need;
	bool forcibly = false;

	if (tank_current != nullptr)
	{
		if (tank_current->tankData->spawnState != TankSpawnState::ACTIVE)
		{
			tank_current = nullptr;
			struct_tank_current = nullptr;
			forcibly = true;
		}
	}

	Vector3 barrelOrigin;
	Vector3 xAxis;
	Vector3 gunDirGlobal;
	HitInfo hitInfo;

	TankSkin *skin = local_tank->skin;
	Mesh *hullMesh = skin->hullMesh;
	Matrix4 hullMatrix;
	hullMatrix.setMatrix(&hullMesh->position, &hullMesh->rotation);



	bool _distance_init = false;

	static uint8_t count;
	if (forcibly == true || count-- == 0)
	{
		tanks_check = true;
		count = 10;

		const Vector3 &local_pos = local_tank->state.pos;
		int32_t local_x_int = local_pos.x;
		int32_t local_y_int = local_pos.y;


		for (;;)
		{
			if (tank_current == nullptr)
			{
				if (_distance_init == false) {
					_distance_init = true;
					this->distanceInit();
				}

				TANKS::StructTank *current = this->getNearTank(50000);
				if (current == nullptr) {
					clearFocus();
					break;
				} else {
					setTankCurrent(current);
				}
			}
			

			if (tank_current->tankData->spawnState != TankSpawnState::ACTIVE) {
				tank_current = nullptr;
				struct_tank_current = nullptr;
				continue;
			}





			Vector3 &target_pos = tank_current->state.pos;
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

			CommonTargetSystemBot *targetSystem = smokyModel->targetSystemBot;
			const double maxDistance = targetSystem->getMaxDistance();
			targetSystem->setMaxDistance(struct_tank_current->distance_to_local + 20);

			bool ok = false;



			{
				BotUtils::calculate_gun_params(skin, &hullMatrix, &this->localBarrelOrigin, direction_center, &barrelOrigin, &xAxis, &gunDirGlobal);
				bool r = targetSystem->getTarget(&barrelOrigin, &gunDirGlobal, &xAxis, local_tank, &hitInfo);
				Tank *target = (Tank*)hitInfo.body;

				if (r == true && target != nullptr)
				{
					if (TANKS::is_tank_enemy(local_tank, target))
					{
						if (target->tankData->spawnState == TankSpawnState::ACTIVE)
						{
							direction_need = direction_center;
							ok = true;
						}
					}
				}
			}





			if (ok == false)
			{
				double dir_step = BotUtils::dir_step(local_x_int, local_y_int, target_x_int, target_y_int, direction_center, hull_direction);

				double direction_left = direction_center;
				double direction_right = direction_center;

				for (uint8_t i = 0; i < 5; i++)
				{
					{
						direction_left = BotUtils::direction_normalize(direction_left - dir_step);
						BotUtils::calculate_gun_params(skin, &hullMatrix, &this->localBarrelOrigin, direction_left, &barrelOrigin, &xAxis, &gunDirGlobal);
						bool r = targetSystem->getTarget(&barrelOrigin, &gunDirGlobal, &xAxis, local_tank, &hitInfo);
						Tank *target = (Tank*)hitInfo.body;
						//WeaponCommonModel_createExplosionEffects((__WeaponCommonModel*)smokyModel->weaponCommonModel, local_tank->tankData->turret, camera, false, smokyModel->hitInfo->position, smokyModel->_gunDirGlobal, nullptr, 1.0);

						if (r == true && target != nullptr)
						{
							if (TANKS::is_tank_enemy(local_tank, target))
							{
								if (target->tankData->spawnState == TankSpawnState::ACTIVE)
								{
									direction_need = direction_left;
									ok = true;
									break;
								}
							}
						}
					}

					{
						direction_right = BotUtils::direction_normalize(direction_right + dir_step);
						BotUtils::calculate_gun_params(skin, &hullMatrix, &localBarrelOrigin, direction_right, &barrelOrigin, &xAxis, &gunDirGlobal);
						bool r = targetSystem->getTarget(&barrelOrigin, &gunDirGlobal, &xAxis, local_tank, &hitInfo);
						Tank *target = (Tank*)hitInfo.body;
						//WeaponCommonModel_createExplosionEffects((__WeaponCommonModel*)smokyModel->weaponCommonModel, local_tank->tankData->turret, camera, false, smokyModel->hitInfo->position, smokyModel->_gunDirGlobal, nullptr, 1.0);
						if (r == true && target != nullptr)
						{
							if (TANKS::is_tank_enemy(local_tank, target))
							{
								if (target->tankData->spawnState == TankSpawnState::ACTIVE)
								{
									direction_need = direction_right;
									ok = true;
									break;
								}
							}
						}
					}
				}
			}

			targetSystem->setMaxDistance(maxDistance);

			if (ok == true)
				break;

			tank_current = nullptr;
			struct_tank_current = nullptr;
		}
	}
	else
	{
		tanks_check = false;
	}



	if (tank_current != nullptr)
	{
		if ((uint32_t)smokyModel->nextReadyTime < time)
		{
			BotUtils::calculate_gun_params(skin, &hullMatrix, &localBarrelOrigin, skin->turretDirection, &barrelOrigin, &xAxis, &gunDirGlobal);
			bool r = smokyModel->targetSystemBot->getTarget(&barrelOrigin, &gunDirGlobal, &xAxis, local_tank, &hitInfo);
			Tank *target = (Tank*)hitInfo.body;
			if (r == true && target != nullptr)
			{
				if (TANKS::is_tank_enemy(local_tank, target))
				{
					if (target->tankData->spawnState == TankSpawnState::ACTIVE)
					{
						if (local_tank->tankData->spawnState == TankSpawnState::ACTIVE)
						{
							shoot(smokyModel, time, skin, &hullMatrix);
						}
					}
				}
			}
		}



		const double v = BotUtils::difference(direction_need, skin->turretDirection);
		const double s = abs(v);

		if (s  > 0.02)
		{
			bool c = s > 0.7; //0.5
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
		}
		else
		{
			body_turn = BodyTurn::NO;

			bool stop = false;
			
			skin->turretDirection = direction_need;
			if (tankControl->is_turret() == true)
			{
				BotUtils::calculate_gun_params(skin, &hullMatrix, &this->localBarrelOrigin, skin->turretDirection, &barrelOrigin, &xAxis, &gunDirGlobal);
				bool r = smokyModel->targetSystemBot->getTarget(&barrelOrigin, &gunDirGlobal, &xAxis, local_tank, &hitInfo);
				Tank *target = (Tank*)hitInfo.body;
				if (r == true && target == tank_current)
				{
					stop = true;
				}
				else if (tankControl->is_left() == true)
				{
					if (v < 0.)
					{
						stop = true;
					}
				}
				else if (tankControl->is_right() == true)
				{
					if (v > 0.)
					{
						stop = true;
					}
				}
				
				if (stop == true)
				{		
					tankControl->turret_stop();
				}
					
				count = 0;
			}
		}

		return;
	}
	else
	{
		body_turn = BodyTurn::NO;
		tankControl->turret_stop();
	}
}


void BotSmoky::deleteTank(Tank *tank)
{
	if (tank_current == tank) {
		tank_current = nullptr;
		struct_tank_current = nullptr;
	}
}
