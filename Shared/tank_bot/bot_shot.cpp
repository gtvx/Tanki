#include "bot_shot.h"
#include "bot_utils.h"
#include "tanks.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"


void BotShot::setTankCurrent(TANKS::StructTank *s_tank)
{
	this->struct_tank_current = s_tank;
	this->tank_current = s_tank->tank;
}

void BotShot::distanceInit()
{
	TANKS::StructTank *my_tank = TANKS::get_my_struct_tank();
	TANKS::StructTank *tank = TANKS::get_tank_first();
	while (tank != nullptr)
	{
		if (tank == my_tank) {
			my_tank->distance_to_local = 0;
		} else {
			double d = BotUtils::distance_d(&tank->tank->state.pos,  &my_tank->tank->state.pos);
			if (d > 2147483640.) {
				tank->distance_to_local = 0xFFFFFFFF;
			} else {
				tank->distance_to_local = (int32_t)d;
			}
		}
		tank->check = false;
		tank = tank->next;
	}
}



TANKS::StructTank* BotShot::getNearTank(uint32_t distance_min)
{
	TANKS::StructTank *tank_min = nullptr;

	TANKS::StructTank *my_tank = TANKS::get_my_struct_tank();
	TANKS::StructTank *tank = TANKS::get_tank_first();
	while (tank != nullptr)
	{
		if (tank->check == false)
		{
			if (tank->distance_to_local < distance_min)
			{
				if (TANKS::is_tank_enemy(my_tank->tank, tank->tank))
				{
					distance_min = tank->distance_to_local;
					tank_min = tank;
				}
				else
				{
					tank->check = true;
				}
			}
		}

		tank = tank->next;
	}

	if (tank_min != nullptr)
	{
		tank_min->check = true;
		return tank_min;
	}

	return nullptr;
}

