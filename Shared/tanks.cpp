#include "tanks.h"
#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <QStringList>
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/object/clientobjectweapon.h"


static TANKS::StructTank *first;
static TANKS::StructTank *end;
static Tank *my_tank;
static TANKS::StructTank *my_struct_tank;

namespace TANKS
{
	//---------------------------------------------------------------------------------------------------
	static void list_add_item(StructTank *item)
	{
		if (first == nullptr)
		{
			//the list is empty
			end = first = item;
			item->prev = nullptr;
		}
		else
		{
			item->prev = end;
			end->next = item;
			end = item;
		}

		item->next = nullptr;
	}

	static void list_remove_item(StructTank *item)
	{
		if (item->prev != nullptr)
			item->prev->next = item->next;
		if (item->next != nullptr)
			item->next->prev = item->prev;

		if (first == item)
			first = item->next;
		if (end == item)
			end = item->prev;
	}




	StructTank* GetStructTank(Tank *tank)
	{
		StructTank *current = first;
		while (current != nullptr) {
			if (current->tank == tank)
				return current;
			current = current->next;
		}
		return nullptr;
	}


	/*
		static MODIFICATION from_turret_modification(flash_class::String *name)
		{
			if (name->compare_end("_m1"))
			return MODIFICATION::M1;

			if (name->compare_end("_m2"))
				return MODIFICATION::M2;

			if (name->compare_end("_m3"))
				return MODIFICATION::M3;

			return MODIFICATION::M0;
		}
	*/

	static TURRET from_turret_name(const QString &name)
	{
		QString n = name.split(QChar('_'))[0];

		if (n.startsWith("smoky"))
			return TURRET::SMOKY;

		if (n.startsWith("twins"))
			return TURRET::TWINS;

		if (n.startsWith("flamethrower"))
			return TURRET::FLAMETHROWER;

		if (n.startsWith("frezee"))
			return TURRET::FREZEE;

		if (n.startsWith("isida"))
			return TURRET::ISIDA;

		if (n.startsWith("railgun"))
			return TURRET::RAILGUN;

		if (n.startsWith("ricochet"))
			return TURRET::RICOCHET;

		if (n.startsWith("terminator"))
			return TURRET::TERMINATOR;

		if (n.startsWith("thunder"))
			return TURRET::THUNDER;

		if (n.startsWith("shaft"))
			return TURRET::SHAFT;

		return TURRET::NONE;
	}

	/*
const char* to_string(TURRET turret)
{
	switch (turret)
	{
		case TURRET::SMOKY:
			return "SMOKY";
		case TURRET::TWINS:
			return "TWINS";
		case TURRET::FLAMETHROWER:
			return "FLAMETHROWER";
		case TURRET::FREZEE:
			return "FREZEE";
		case TURRET::ISIDA:
			return "ISIDA";
		case TURRET::RAILGUN:
			return "RAILGUN";
		case TURRET::RICOCHET:
			return "RICOCHET";
		case TURRET::TERMINATOR:
			return "TERMINATOR";
		case TURRET::THUNDER:
			return "THUNDER";
		case TURRET::SHAFT:
			return "SHAFT";
		case TURRET::ERROR:
			return "ERROR";
	}
	return "";
}

const char* to_string(MODIFICATION modification)
{
	switch (modification)
	{
		case MODIFICATION::M0:
			return "M0";
		case MODIFICATION::M1:
			return "M1";
		case MODIFICATION::M2:
			return "M2";
		case MODIFICATION::M3:
			return "M3";
	}
	return "";
}
*/



	void tank_add(Tank *tank)
	{
		StructTank *s = GetStructTank(tank);
		if (s != nullptr) {
			throw 54532;
		}

		s = new StructTank;
		s->tank = tank;
		s->tank_data = tank->tankData;
		s->user = tank->tankData->user;
		s->next = nullptr;
		s->filter_1_time = 0;

		s->name = new QString;
		*s->name = s->tank->tankData->user->getId();
		s->cord_error = false;


		QString turret_name = s->tank_data->turret->getId();

		s->turret = from_turret_name(turret_name);
		//s->modification = from_turret_modification(turret_name);

		if (s->turret == TURRET::NONE) {
			delete s->name;
			delete s;
			throw 246245;
		}

		//QString t;
		//turret_name->readString(&t, false, false);
		//qDebug() << to_string(s->turret) << to_string(s->modification) << t;

		if (s->tank->tankData->local == true) {
			my_tank = s->tank;
			my_struct_tank = s;
		}

		//__Vector3 *position = s->tank->state->pos;
		//s->server_position_x = position->x;
		//s->server_position_y = position->y;
		//s->server_position_z = position->z;

		list_add_item(s);

		tank->structTank = s;
		end = s;

		return;
	}

	void tank_delete(Tank *tank)
	{
		StructTank *s = GetStructTank(tank);
		if (s == nullptr)
			return;

		if (s->tank == my_tank) {
			my_tank = nullptr;
			my_struct_tank = nullptr;
		}

		delete s->name;
		list_remove_item(s);

		delete s;
	}

	void clear()
	{
		StructTank *current = first;
		while (current != nullptr) {
			StructTank *next = current->next;
			delete current->name;
			delete current;
			current = next;
		}
		first = nullptr;
		end = nullptr;
		my_tank = nullptr;
		my_struct_tank = nullptr;
	}


	StructTank* get_tank_first()
	{
		return first;
	}

	StructTank* get_tank_end()
	{
		return end;
	}


	//---------------------------------------------------------------------------------------------------

	bool is_tank_enemy(Tank *tank_a, Tank *tank_b)
	{
		if (tank_a == tank_b)
			return false;

		BattleTeamType team_a = tank_a->tankData->teamType;

		if (team_a == BattleTeamType::NONE)
		{
			return true;
		}
		else
		{
			return team_a != tank_b->tankData->teamType;
		}
	}


	bool is_tank_green(Tank *tank)
	{
		return tank->tankData->teamType == BattleTeamType::NONE;
	}

	bool is_rank_red(Tank *tank)
	{
		return tank->tankData->teamType == BattleTeamType::RED;
	}

	bool is_tank_blue(Tank *tank)
	{
		return tank->tankData->teamType == BattleTeamType::BLUE;
	}

	QString get_tank_team(Tank *tank)
	{
		BattleTeamType teamType = tank->tankData->teamType;
		if (teamType == BattleTeamType::NONE) {
			return "GREEN";
		} else if (teamType == BattleTeamType::RED) {
			return "RED";
		} else if (teamType == BattleTeamType::BLUE) {
			return "BLUE";
		}

		return QString();
	}
	//---------------------------------------------------------------------------------------------------

	Tank* get_my_tank()
	{
		return my_tank;
	}

	StructTank* get_my_struct_tank()
	{
		return my_struct_tank;
	}

	//---------------------------------------------------------------------------------------------------
}
