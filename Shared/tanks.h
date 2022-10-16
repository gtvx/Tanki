#ifndef TANKS_H
#define TANKS_H

#include <stdint.h>
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"

struct Tank;
struct TankData;
struct ClientObjectUser;
class QString;

namespace TANKS
{
	enum class TURRET
	{
		SMOKY,
		TWINS,
		FLAMETHROWER,
		FREZEE,
		ISIDA,
		RAILGUN,
		RICOCHET,
		TERMINATOR,
		THUNDER,
		SHAFT,
		NONE,
	};


	enum class MODIFICATION
	{
		M0,
		M1,
		M2,
		M3,
	};


	struct StructTank
	{
		Tank *tank;
		TankData *tank_data;
		ClientObjectUser *user;
		QString *name;
		StructTank *prev;
		StructTank *next;
		//double server_position_x;
		//double server_position_y;
		//double server_position_z;
		TURRET turret;
		//MODIFICATION modification;
		uint32_t distance_to_local;
		uint32_t filter_1_time;
		bool check;
		bool cord_error;
	};


	StructTank* get_tank_first();
	StructTank* get_tank_end();
	bool is_tank_enemy(Tank *tank_a, Tank *tank_b);
	bool is_tank_green(Tank *tank);
	bool is_tank_red(Tank *tank);
	bool is_tank_blue(Tank *tank);
	QString get_tank_team(Tank *tank);

	Tank* get_my_tank();
	StructTank* get_my_struct_tank();


	void tank_add(Tank *tank);
	void tank_delete(Tank *tank);
	void clear();
}

#endif // TANKS_H
