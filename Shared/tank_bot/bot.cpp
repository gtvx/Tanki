#include "bot.h"
#include "tanks.h"
#include "bot_smoky.h"
#include "bot_flamethrower.h"
#include "tank_control.h"
#include "service.h"
#include "botsettings.h"
#include "map_points.h"
#include "tanks.h"
#include "map_list_path.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/track.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/ctf/ctfmodel.h"
#include "alternativa/tanks/models/ctf/ctfflag.h"
#include "bot_utils.h"
#include "path_normalize.h"
#include "astar.h"
#include "map_points_file.h"
#include "alternativa/tanks/models/dom/dommodel.h"
#include "alternativa/tanks/models/dom/dompoint.h"
#include "alternativa/tanks/engine3d/animatedsprite3d.h"
#include "gametime.h"
#include "mymath.h"
#include "bot_points.h"
#include <QRandomGenerator>


static uint32_t random(uint32_t lowest, uint32_t highest)
{
	return (QRandomGenerator::global()->generate() % ((highest - lowest)+1)) + lowest;
}


static inline bool find_xp(MapPoint *point, uint8_t length)
{
	uint8_t count = 0;
	for (;;)
	{
		point = point->XP;
		if (point == nullptr)
			return false;
		if (point->locked == true)
			return false;
		if (++count == length)
			return true;
	}
}

static inline bool find_xm(MapPoint *point, uint8_t length)
{
	uint8_t count = 0;
	for (;;)
	{
		point = point->XM;
		if (point == nullptr)
			return false;
		if (point->locked == true)
			return false;
		if (++count == length)
			return true;
	}
}


static inline bool find_yp(MapPoint *point, uint8_t length)
{
	uint8_t count = 0;
	for (;;)
	{
		point = point->YP;
		if (point == nullptr)
			return false;
		if (point->locked == true)
			return false;
		if (++count == length)
			return true;
	}
}

static inline bool find_ym(MapPoint *point, uint8_t length)
{
	uint8_t count = 0;
	for (;;)
	{
		point = point->YM;
		if (point == nullptr)
			return false;
		if (point->locked == true)
			return false;
		if (++count == length)
			return true;
	}
}


static inline bool find_xp_yp(MapPoint *point, uint8_t length)
{
	uint8_t count = 0;
	for (;;)
	{
		point = point->XP_YP;
		if (point == nullptr)
			return false;
		if (point->locked == true)
			return false;
		if (++count == length)
			return true;
	}
}

static inline bool find_xp_ym(MapPoint *point, uint8_t length)
{
	uint8_t count = 0;
	for (;;)
	{
		point = point->XP_YM;
		if (point == nullptr)
			return false;
		if (point->locked == true)
			return false;
		if (++count == length)
			return true;
	}
}

static inline bool find_xm_yp(MapPoint *point, uint8_t length)
{
	uint8_t count = 0;
	for (;;)
	{
		point = point->XM_YP;
		if (point == nullptr)
			return false;
		if (point->locked == true)
			return false;
		if (++count == length)
			return true;
	}
}

static bool find_xm_ym(MapPoint *point, uint8_t length)
{
	uint8_t count = 0;
	for (;;)
	{
		point = point->XM_YM;
		if (point == nullptr)
			return false;
		if (point->locked == true)
			return false;
		if (++count == length)
			return true;
	}
}



class BotPrivate
{
public:
	enum class BotStatus
	{
		FREE,
		TAKE_THE_ENEME_FLAG,
		ELIVER_THE_ENEMY_FLAG,
		CP,
		TANK,
		RANDOM_1,
		RANDOM_2,
		RANDOM_3,
	};


	BotStatus bot_status;
	bool enable;

	BotShot *bot_shot;
	TANKS::StructTank *my_struct_tank;
	Tank *my_tank;
	TankControl *tankControl;
	TankModel *tankModel;
	BattlefieldModel *battlefieldModel;
	BotSettings *botSettings;
	uint32_t last_time_focus_tanks;
	uint32_t last_time_create_path;
	TanksCollisionDetector *collisionDetector;
	MapPoints map;
	MapListPath map_path;
	Tank *tank_path;
	CTFModel *ctfModel;
	DOMModel *domModel;
	uint32_t time_spawn_local;
	bool block_body_turn;
	DomPoint *dom_point_current;
	CTFFlagState last_status_flag;
	bool stop_motion_1;
	bool stop_motion_2;
	uint8_t create_first_count;
	uint32_t auto_kill_last_time;
	uint32_t auto_kill_last_time_speed;
	AStar astar;


	BotPrivate(TankModel *tankModel, TANKS::StructTank *structTank, BotSettings *botSettings, TanksCollisionDetector *collisionDetector)
	{
		this->enable = false;
		this->collisionDetector = collisionDetector;
		this->map.collisionDetector = collisionDetector;
		this->my_struct_tank = structTank;
		this->my_tank = structTank->tank;
		this->botSettings = botSettings;
		this->bot_status = BotStatus::FREE;
		this->tank_path = nullptr;
		this->stop_motion_1 = false;
		this->stop_motion_2 = false;
		this->create_first_count = 0;
		this->auto_kill_last_time = 0;
		this->auto_kill_last_time_speed = 0;
		this->dom_point_current = nullptr;

		this->tankModel = tankModel;
		this->battlefieldModel = Service::getBattlefieldModel();
		this->ctfModel = Service::getCTFModel();
		this->domModel = Service::getDOMModel();

		this->tankControl = new TankControl(tankModel, my_struct_tank->tank);
		this->last_time_focus_tanks = 0;
		this->last_time_create_path = 0;
		this->time_spawn_local = INT_MAX;
		this->block_body_turn = false;

		//this->botSettings->target_tank = true;
		//this->botSettings->body_turn = true;

		this->last_status_flag = CTFFlagState::AT_BASE;


		switch (my_struct_tank->turret)
		{
			case TANKS::TURRET::SMOKY:
				bot_shot = new BotSmoky(tankControl);
				if (bot_shot->init() == false) {
					delete bot_shot;
					bot_shot = nullptr;
				}
				break;
			case TANKS::TURRET::FLAMETHROWER:
				bot_shot = new BotFlamethrower(tankControl);
				if (bot_shot->init() == false) {
					delete bot_shot;
					bot_shot = nullptr;
				}
				break;
			default:
				bot_shot = nullptr;
				break;
		}
	}

	~BotPrivate()
	{
		delete bot_shot;
		delete tankControl;
	}

	void updateCommands()
	{
		if (bot_status == BotStatus::FREE) {
			map_list_path_clear(map_path);
			return;
		}

		if (botSettings->capture_points == false || domModel == nullptr) {
			if (bot_status == BotStatus::CP) {
				bot_status = BotStatus::FREE;
				map_list_path_clear(map_path);
			}
		}

		if (botSettings->carry_flag == false || ctfModel == nullptr) {
			if (bot_status == BotStatus::TAKE_THE_ENEME_FLAG || bot_status == BotStatus::ELIVER_THE_ENEMY_FLAG) {
				bot_status = BotStatus::FREE;
				map_list_path_clear(map_path);
			}
		}

		if (botSettings->target_tank == false) {
			if (bot_status == BotStatus::TANK) {
				bot_status = BotStatus::FREE;
				map_list_path_clear(map_path);
			}
		}

		if (botSettings->random_path == false) {
			if (bot_status == BotStatus::RANDOM_1 ||
				bot_status == BotStatus::RANDOM_2 ||
				bot_status == BotStatus::RANDOM_3) {
				bot_status = BotStatus::FREE;
				map_list_path_clear(map_path);
			}
		}


	}

	bool test_cord(double v)
	{
		if (MyMath::isNaN(v))
			return false;
		return MyMath::abs(v) < 100000000.;
	}


	void deleteTank(Tank *tank)
	{
		if (bot_shot != nullptr)
			bot_shot->deleteTank(tank);
		if (tank_path == tank) {
			tank_path = nullptr;
		}
	}

	void tank_stop()
	{
		tank_path = nullptr;
		map_list_path_clear(map_path);
		tankControl->stop_motion();
		tankControl->stop_turn();
		tankControl->apply();
	}


	void spawnTank(uint32_t time)
	{
		time_spawn_local = time;
	}


	void setEnable(bool status)
	{
		this->enable = status;
		if (enable == true) {
			if (map.length == 0)
				this->loadPoints();
			auto_kill_last_time_speed = GetGameTime();
		} else {
			tank_path = nullptr;
			map_list_path_clear(map_path);
			tankControl->stop();
			tankControl->apply();
		}
	}

	void savePoints()
	{
		bot_points_save(battlefieldModel, &map);
	}

	void loadPoints()
	{
		map.clear();
		bot_points_load(battlefieldModel, &map);
	}

	MapPoint* get_near_tank_point(TANKS::StructTank *&other_tank)
	{
		Tank *local_tank = my_tank;

		Vector3 &local_pos = local_tank->state.pos;

		int32_t local_pos_x = local_pos.x;
		int32_t local_pos_y = local_pos.y;
		int32_t local_pos_z = local_pos.z;

		uint32_t distance_min = 0xFFFFFFFF;
		TANKS::StructTank *tank_min = nullptr;

		TANKS::StructTank *current_tank = TANKS::get_tank_first();


		while (current_tank != nullptr)
		{
			if (current_tank->cord_error == false && TANKS::is_tank_enemy(local_tank, current_tank->tank) && current_tank->filter_1_time == 0)
			{
				if (current_tank->tank_data->spawnState == TankSpawnState::ACTIVE)
				{
					const Vector3 &pos = current_tank->tank->state.pos;
					uint32_t distance = BotUtils::int_xyz_to_distance_plain(local_pos_x, local_pos_y, local_pos_z, pos.x, pos.y, pos.z);
					if (distance < distance_min)
					{
						distance_min = distance;
						tank_min = current_tank;
					}
				}
			}

			current_tank = current_tank->next;
		}

		if (tank_min == nullptr)
			return nullptr;

		other_tank = tank_min;
		const Vector3 &pos = tank_min->tank->state.pos;
		uint32_t distance_out;
		return points_get_point(pos.x, pos.y, pos.z, distance_out);
	}


	MapPoint* get_near_point_2(double x, double y, double z)
	{
		int32_t _x = x;
		int32_t _y = y;
		int32_t _z = z;

		MapPoint *point = map.point_first;
		MapPoint *point_min_no_lock = nullptr;
		uint32_t distance_min_no_lock = UINT_MAX;


		while (point != nullptr)
		{
			uint32_t distance = BotUtils::int_xyz_to_distance_plain(point->x, point->y, point->z, _x, _y, _z);

			if (point->locked == false)
			{
				if (distance < distance_min_no_lock) {
					distance_min_no_lock = distance;
					point_min_no_lock = point;
				}
			}

			point = point->next;
		}

		if (distance_min_no_lock > 250)
			return nullptr;

		return point_min_no_lock;
	}



	void map_points_astar(MapPoint *start, MapPoint *end, MapListPath &map_path)
	{
		if (map.point_first == nullptr)
			return;

		astar.find(start, end, 10000000, map_path);
	}


	void map_points_astar_2(MapPoint *point_end, MapListPath &map_path)
	{
		if (map.point_first == nullptr)
			return;

		Tank *tank = TANKS::get_my_tank();
		if (tank == nullptr)
			return;

		const Vector3 &pos = tank->state.pos;

		MapPoint *point_min_start = get_near_point_2(pos.x, pos.y, pos.z);
		if (point_min_start == nullptr) {
			map_list_path_clear(map_path);
			return;
		}

		astar.find(point_min_start, point_end, 10000000, map_path);
	}



	void path_create_first(bool generate)
	{
		if (map_path.size == 0)
			return;

		const Vector3 &pos = my_tank->state.pos;

		MapPoint *my_point = get_near_point_2(pos.x, pos.y, pos.z);
		if (my_point == nullptr)
			return;

		MapListPathNode *node = map_path.first;
		MapListPathNode *node_last = nullptr;

		while (node != nullptr)
		{
			MapPoint *point = node->point;

			if (map_point_line_check(my_point, point) == true)
			{
				if (node_last != nullptr)
					map_list_path_remove_first(map_path);

				node_last = node;
				node = node->next;
			}
			else
				break;
		}

		if (generate == true && node_last == nullptr)
		{
			MapListPath map_path_2;
			map_points_astar(my_point, node->point, map_path_2);
			if (map_path_2.size > 1)
			{
				MapListPathNode *node = map_path_2.end;
				node = node->prev;
				MapListPathNode *end = map_path.first;
				if (end->next != nullptr)
					end = end->next;

				while (node != nullptr)
				{
					map_list_path_insert_first(map_path, node->point);
					node = node->prev;
				}

				path_normalize(map_path, end);
			}

			map_list_path_clear(map_path_2);

			if (map_path.size > 1)
			{
				MapListPathNode *node = map_path.first;
				MapListPathNode *node_last = nullptr;
				while (node != nullptr)
				{
					MapPoint *point = node->point;

					if (map_point_line_check(my_point, point) == true)
					{
						if (node_last != nullptr)
							map_list_path_remove_first(map_path);

						node_last = node;
						node = node->next;
					}
					else
						break;
				}
			}

		}
	}


	MapPoint* points_get_point(double x, double y, double z, uint32_t &distance_out)
	{
		int32_t _x = x;
		int32_t _y = y;
		int32_t _z = z;

		MapPoint *point = map.point_first;
		MapPoint *point_min = point;

		uint32_t distance_min = 0xFFFFFFFF;

		while (point != nullptr)
		{
			uint32_t distance = BotUtils::int_xyz_to_distance_plain(point->x, point->y, point->z, _x, _y, _z);

			if (distance < distance_min) {
				distance_min = distance;
				point_min = point;
			}

			point = point->next;
		}
		distance_out = distance_min;

		return point_min;
	}

	bool create_path(double x, double y, double z)
	{
		Tank *tank = TANKS::get_my_tank();
		if (tank == nullptr) {
			map_list_path_clear(map_path);
			return false;
		}

		Vector3 *pos = &tank->state.pos;

		uint32_t distance = BotUtils::int_xyz_to_distance_plain(pos->x, pos->y, pos->z, x, y, z);

		if (distance < 150) {
			map_list_path_clear(map_path);
			return false;
		}

		uint32_t distance_out;
		MapPoint *point = points_get_point(x, y, z, distance_out);
		map_points_astar_2(point, map_path);
		return true;
	}


	void path_random()
	{
		Tank *tank = TANKS::get_my_tank();
		if (tank == nullptr)
			return;
		const Vector3 &pos = tank->state.pos;
		MapPoint *my_point = get_near_point_2(pos.x, pos.y, pos.z);
		if (my_point == nullptr)
			return;

		bool dir_xp_block = false;
		bool dir_xm_block = false;
		bool dir_yp_block = false;
		bool dir_ym_block = false;


		MapPoint *current_point = my_point;

		uint32_t steps = random(8, 20);

		uint8_t c = 0;
		for (int i = 0; i < 100; i++)
		{
			uint32_t value = random(0, 3);
			if (value == 0) {
				if (dir_xp_block == false)
				{
					MapPoint *p = current_point;
					MapPoint *ok = p;
					for (uint32_t i = 0; i < steps; i++) {
						if (p == nullptr)
							break;
						if (p->locked == true)
							break;
						ok = p;
						p = p->XP;
					}
					current_point = ok;
					dir_xm_block = true;
					c++;
				}
			} else if (value == 1) {
				if (dir_xm_block == false)
				{
					MapPoint *p = current_point;
					MapPoint *ok = p;
					for (uint32_t i = 0; i < steps; i++) {
						if (p == nullptr)
							break;
						if (p->locked == true)
							break;
						ok = p;
						p = p->XM;
					}
					current_point = ok;
					dir_xp_block = true;
					c++;
				}
			} else if (value == 2) {
				if (dir_yp_block == false)
				{
					MapPoint *p = current_point;
					MapPoint *ok = p;
					for (uint32_t i = 0; i < steps; i++) {
						if (p == nullptr)
							break;
						if (p->locked == true)
							break;
						ok = p;
						p = p->YP;
					}
					current_point = ok;
					dir_ym_block = true;
					c++;
				}
			} else if (value == 3) {
				if (dir_ym_block == false)
				{
					MapPoint *p = current_point;
					MapPoint *ok = p;
					for (uint32_t i = 0; i < steps; i++) {
						if (p == nullptr)
							break;
						if (p->locked == true)
							break;
						ok = p;
						p = p->YM;
					}
					current_point = ok;
					dir_yp_block = true;
					c++;
				}
			}

			if (c == 2) {
				map_points_astar(my_point, current_point, map_path);
				return;
			}
		}
	}



	bool create_path(MapPoint *point)
	{
		Tank *tank = TANKS::get_my_tank();
		if (tank == nullptr) {
			map_list_path_clear(map_path);
			return false;
		}

		const Vector3 &pos = tank->state.pos;

		uint32_t distance = BotUtils::int_xyz_to_distance_plain(pos.x, pos.y, pos.z, point->x, point->y, point->z);

		if (distance < 150) {
			map_list_path_clear(map_path);
			return false;
		}

		map_points_astar_2(point, map_path);
		return true;
	}

	inline int32_t dom_get_filling(DomPoint *point, bool team_red)
	{
		if (team_red == true)
			return -point->clientProgress;
		else
			return point->clientProgress;
	}


	void run(uint32_t time)
	{
		Tank *my_tank = my_struct_tank->tank;


		if (my_tank->tankData->spawnState == TankSpawnState::SUICIDE)
			return;

		const uint32_t current_time = time;


		if (my_tank->tankData->enabled == false) {
			tankControl->stop();
			tankControl->apply();
			return;
		}

		{
			TANKS::StructTank *tank = TANKS::get_tank_first();
			while (tank != nullptr)
			{
				const Vector3 &pos = tank->tank->state.pos;
				tank->cord_error = !test_cord(pos.x) || !test_cord(pos.y) || !test_cord(pos.z);
				tank = tank->next;
			}
		}

		if (my_tank->tankData->spawnState == TankSpawnState::NEWCOME/* && bot_status != BotStatus::RANDOM_1*/)
		{
			if (my_tank->new_contact_normal == true)
			{
				if (map_path.size == 0)
				{
					path_random();
				}
				if (map_path.size != 0)
				{
					bot_status = BotStatus::RANDOM_1;
					path_normalize(map_path, map_path.end);
				}
			}
		}


		if (botSettings->auto_kill == true)
		{
			if (my_tank->leftTrack->lastContactsNum == 0 && my_tank->rightTrack->lastContactsNum == 0)
			{
				const Vector3 &velocity = my_tank->state.velocity;

				double speed = MyMath::abs(velocity.x) + MyMath::abs(velocity.y) + MyMath::abs(velocity.z);

				if (speed < 100.)
				{
					if ((auto_kill_last_time + 5000) < current_time && (time_spawn_local + 5000) < current_time)
					{
						tankModel->startSuicideCountdown();
					}
				}
				else
				{
					auto_kill_last_time = current_time;
				}
			}
			else
			{
				auto_kill_last_time = current_time;

				if (map_path.size != 0)
				{
					const Vector3 &velocity = my_tank->state.velocity;
					double speed = MyMath::abs(velocity.x) + MyMath::abs(velocity.y) + MyMath::abs(velocity.z);

					if (speed < 50.)
					{
						if ((auto_kill_last_time_speed + 10000) < current_time && (time_spawn_local + 5000) < current_time)
						{
							tankModel->startSuicideCountdown();
						}
					}
					else
					{
						auto_kill_last_time_speed = current_time;
					}
				}
				else
				{
					auto_kill_last_time_speed = current_time;
				}
			}
		}
		else
		{
			auto_kill_last_time_speed = current_time;
		}



		bool focus_tank = false;
		bool block_tank_path = false;


		if (botSettings->shoot == true)
		{
			if (bot_shot != nullptr)
			{
				bot_shot->processing(time);
				focus_tank = bot_shot->isTankFocus();

				if (focus_tank == true)
					last_time_focus_tanks = time;

				block_tank_path = bot_shot->tanksCheck() == false;
			}
		}



		bool block_create_path = false;


		if (botSettings->capture_points == true && domModel != nullptr)
		{
			BattleTeamType my_team_type = my_struct_tank->tank->tankData->teamType;
			if (my_team_type != BattleTeamType::NONE)
			{
				bool team_red = my_team_type == BattleTeamType::RED;

				if (domModel != nullptr)
				{
					if (bot_status == BotStatus::CP && dom_point_current != nullptr)
					{
						if (dom_point_current->capture == false && map_path.size == 0)
						{
							bot_status = BotStatus::FREE;
							dom_point_current = nullptr;
							tank_stop();
						}
						else
						{
							if (dom_get_filling(dom_point_current, team_red) >= 100) {
								dom_point_current = nullptr;
								bot_status = BotStatus::FREE;
								tank_stop();
							} else {
								if (dom_point_current->capture == true)
								{
									tank_stop();
								}
								else
								{
									const Vector3 &pos = my_tank->state.pos;
									const Vector3 &pos_dom = dom_point_current->pos;
									uint32_t distance = BotUtils::int_xyz_to_distance_plain(pos.x, pos.y, pos.z, pos_dom.x, pos_dom.y, pos_dom.z);
									if (distance < 150) {
										tank_stop();
									}
								}
							}
						}
					} else if (map_path.size == 0) {
						bot_status = BotStatus::FREE;
						dom_point_current = nullptr;
						tank_stop();
					}

					if (bot_status == BotStatus::FREE)
					{
						uint32_t distance_min = 0xFFFFFFFF;
						DomPoint *point_min = nullptr;

						for (int i = 0; i < domModel->points.count(); i++)
						{
							DomPoint *point = domModel->points.at(i);
							int32_t fillin = dom_get_filling(point, team_red);

							if (fillin < 100) {
								const Vector3 &tank_pos = my_tank->state.pos;
								const Vector3 &point_pos = point->pos;
								uint32_t distance = BotUtils::int_xyz_to_distance_plain(tank_pos.x, tank_pos.y, tank_pos.z, point_pos.x, point_pos.y, point_pos.z);
								if (distance < distance_min) {
									distance_min = distance;
									point_min = point;
								}
							}
						}

						if (point_min != nullptr)
						{
							dom_point_current = point_min;
							const Vector3 &pos = point_min->pos;
							if (create_path(pos.x, pos.y, pos.z - 210) == true) {
								bot_status = BotStatus::CP;
								path_normalize(map_path, map_path.end);
							}
						}
					}
				}
			}
		}




		if (botSettings->carry_flag == true && ctfModel != nullptr)
		{
			CTFFlag *flag_red = ctfModel->flag_red;
			CTFFlag *flag_blue = ctfModel->flag_blue;


			BattleTeamType my_team_type= my_struct_tank->tank->tankData->teamType;
			if (my_team_type != BattleTeamType::NONE)
			{
				CTFFlag *our_flag;
				CTFFlag *hostile_flag;

				if (my_team_type == BattleTeamType::RED)
				{
					our_flag = flag_red;
					hostile_flag = flag_blue;
				} else if (my_team_type == BattleTeamType::BLUE)
				{
					our_flag = flag_blue;
					hostile_flag = flag_red;
				} else
					return;

				if (map_path.size == 0)
				{
					bot_status = BotStatus::FREE;
					tank_stop();
				}


				CTFFlagState status_flag = hostile_flag->getState();


				if (status_flag == CTFFlagState::AT_BASE)
				{
					block_create_path = true;
					if (status_flag != last_status_flag || bot_status == BotStatus::FREE)
					{
						last_status_flag = status_flag;

						const Vector3 &end = hostile_flag->_basePosition;
						if (create_path(end.x, end.y, end.z) == true) {
							bot_status = BotStatus::TAKE_THE_ENEME_FLAG;
							path_normalize(map_path, map_path.end);
						}
					}

				} else if (status_flag == CTFFlagState::CARRIED)
				{
					bool translating_flag = hostile_flag->getCarrierData() == my_tank->tankData;
					block_create_path = translating_flag;

					if (status_flag != last_status_flag || bot_status == BotStatus::FREE)
					{
						last_status_flag = status_flag;

						if (translating_flag == true)
						{
							Vector3 &end = our_flag->_basePosition;
							if (create_path(end.x, end.y, end.z) == true) {
								bot_status = BotStatus::ELIVER_THE_ENEMY_FLAG;
								path_normalize(map_path, map_path.end);
							}
						}
						else
						{
							map_list_path_clear(map_path);
							bot_status = BotStatus::FREE;
						}
					}

				} else if (status_flag == CTFFlagState::DROPPED)
				{
					block_create_path = true;
					if (status_flag != last_status_flag || bot_status == BotStatus::FREE)
					{
						last_status_flag = status_flag;

						//Object3D *hostile_skin = (Object3D*)hostile_flag->skin;
						const Vector3 &hostile = hostile_flag->skin->position;
						if (create_path(hostile.x, hostile.y, hostile.z) == true) {
							bot_status = BotStatus::TAKE_THE_ENEME_FLAG;
							path_normalize(map_path, map_path.end);
						}
					}
				}
			}
		}

		if (bot_status == BotStatus::TANK && focus_tank == true)
		{
			bot_status = BotStatus::FREE;
			tank_stop();
		}

		{
			TANKS::StructTank *current_tank = TANKS::get_tank_first();

			while (current_tank != nullptr)
			{
				if (current_tank->filter_1_time != 0)
				{
					if (current_tank->filter_1_time < current_time)
					{
						current_tank->filter_1_time = 0;
					}
				}
				current_tank = current_tank->next;
			}
		}


		if (block_create_path == false)
		{
			if (bot_status == BotStatus::FREE)
			{
				if (botSettings->target_tank == true && focus_tank == false && block_tank_path == false)
				{
					TANKS::StructTank *tank;
					MapPoint *point_end = get_near_tank_point(tank);

					if (point_end != nullptr)
					{
						bot_status = BotStatus::TANK;
						last_time_create_path = time;
						block_body_turn = true;

						bool c = create_path(point_end);
						if (c == false || map_path.size < 4)
						{
							tank->filter_1_time = current_time + 20000;
							tank_stop();
						}
						else
						{
							path_normalize(map_path, map_path.end);
							tank_path = tank->tank;
						}
					}
				}
			}


			if (bot_status == BotStatus::FREE)
			{
				if (botSettings->random_path == true)
				{
					if (map_path.size == 0)
						path_random();
					if (map_path.size != 0) {
						bot_status = BotStatus::RANDOM_2;
						path_normalize(map_path, map_path.end);
					}
				}
			}


			if (bot_status == BotStatus::FREE)
			{
				if (my_tank->normal_contact_new == true && focus_tank == false)
				{
					if (map_path.size == 0)
						path_random();
					if (map_path.size != 0) {
						bot_status = BotStatus::RANDOM_3;
						path_normalize(map_path, map_path.end);
					}
				}
			}
		}


		if (bot_status == BotStatus::RANDOM_1 && my_tank->new_contact_normal == false)
		{
			bot_status = BotStatus::FREE;
			tank_stop();
		}

		if (bot_status == BotStatus::RANDOM_3 && my_tank->normal_contact_new == false)
		{
			bot_status = BotStatus::FREE;
			tank_stop();
		}

		if (bot_status == BotStatus::TANK)
		{
			if (!(tank_path != nullptr && tank_path->tankData->spawnState == TankSpawnState::ACTIVE))
			{
				bot_status = BotStatus::FREE;
				tank_stop();
			}
		}

		if (map_path.size != 0)
		{
			if (create_first_count++ == 5) {
				create_first_count = 0;
				path_create_first(true);
			}
		}

		bool body_turn = false;

		if (bot_status == BotStatus::FREE)
		{
			if (block_body_turn == true)
			{
				if (current_time > (last_time_create_path + 1500))
					block_body_turn = false;
			}


			if (botSettings->body_turn == true && block_body_turn == false && bot_shot != nullptr)
			{
				switch (bot_shot->getBodyTurn())
				{
					case BotShot::BodyTurn::NO:
						tankControl->stop_turn();
						break;
					case BotShot::BodyTurn::LEFT:
						if (tankControl->is_right() == true) {
							tankControl->stop_turn();
						} else {
							tankControl->left();
							body_turn = true;
						}
						break;
					case BotShot::BodyTurn::RIGHT:
						if (tankControl->is_left() == true) {
							tankControl->stop_turn();
						} else {
							tankControl->right();
							body_turn = true;
						}
						break;
				}
			}
			else
			{
				tankControl->stop_turn();
			}
		}


		bool b = false;

		const uint32_t leftContactsNum = my_tank->leftTrack->lastContactsNum;
		const uint32_t rightContactsNum = my_tank->rightTrack->lastContactsNum;

		if (leftContactsNum > 0 || rightContactsNum > 0)
		{
			double rotationX = my_tank->skin->hullMesh->rotation.x;
			if (rotationX < -0.8) {
				tankControl->forward();
				b = true;
			} else if (rotationX > 0.8) {
				tankControl->back();
				b = true;
			}
		}

		if (b == false)
		{
			if (map_path.size != 0 && bot_motion() == false) {

			} else {
				tankControl->stop_motion();
				if (body_turn == false)
					tankControl->stop_turn();
			}
		}


		if (botSettings->shoot == true && focus_tank == true)
		{

		} else if (map_path.size == 0 && dom_point_current == nullptr)
		{
			bot_status = BotStatus::FREE;
		}


		if (focus_tank == false)
		{
			if ((last_time_focus_tanks + 2000) < current_time)
			{
				double turretDirection = my_tank->skin->turretDirection;

				if (tankControl->is_forward() == true) {
					if (turretDirection != 0) {
						if (tankControl->is_turret_left_right() == true) {
							tankControl->turret_stop();
						} else {
							tankControl->turret_center();
						}
					}
				} else if (tankControl->is_back() == true) {
					if (turretDirection >= 0) {
						if (turretDirection < 3.12) {
							tankControl->turret_left();
						} else {
							tankControl->turret_stop();
						}
					} else {
						if (turretDirection > -3.12) {
							tankControl->turret_right();
						} else {
							tankControl->turret_stop();
						}
					}
				} else {
					tankControl->turret_stop();
				}
			}
		}


		tankControl->apply();
	}






	bool bot_motion()
	{
		Tank *tank = my_tank;
		if (tank == nullptr)
			return true;

		const uint32_t leftContactsNum = tank->leftTrack->lastContactsNum;
		const uint32_t rightContactsNum = tank->rightTrack->lastContactsNum;

		if (leftContactsNum == 0 && rightContactsNum == 0)
		{
			const Vector3 &velocity = tank->state.velocity;
			double speed = MyMath::abs(velocity.x) + MyMath::abs(velocity.y) + MyMath::abs(velocity.z);
			if (speed < 10.)
				return true;
		}

		//bool stop_2 = leftContactsNum < 4;
		//if (stop_2 == false)
		//stop_2 = rightContactsNum < 4;


		Mesh *hullMesh = tank->skin->hullMesh;

		int32_t my_x = hullMesh->position.x;
		int32_t my_y = hullMesh->position.y;
		int32_t my_z = hullMesh->position.z;


		/*
		uint32 distance_min = UINT_MAX;
		int n = 0;

		MapListPathNode *node = map_path.first;

		while (node != nullptr)
		{
			MapPoint *point = node->point;
			uint32 distance = distance_x_y_z(my_x, my_y, my_z, point->x, point->y, point->z);
			if (distance < distance_min) {
				distance_min = distance;
				n++;
			}
			node = node->next;
		}

		for (int i = 0; i < n-1; i++)
			map_list_path_remove_first(map_path);
		*/


		if (map_path.size == 0)
			return true;

		uint32_t distance_x_y_max = 150;

		bool f = false;

		for (;;)
		{
			uint32_t length = map_path.size;
			if (length == 0)
				return true;

			if (length == 1)
				distance_x_y_max = 150;

			MapPoint *point = map_path.first->point;
			uint32_t _distance_x_y = BotUtils::int_xy_to_distance_plain(my_x, my_y, point->x, point->y);
			uint32_t _distance_z = BotUtils::int_z_to_distance_plain(my_z, point->z);
			if (_distance_x_y < distance_x_y_max && _distance_z < 170) {
				map_list_path_remove_first(map_path);
				f = true;
			} else
				break;
		}

		if (f == true)
			path_create_first(false);

		const Vector3 &velocity = tank->state.velocity;
		double speed = MyMath::sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y) + (velocity.z * velocity.z));

		MapPoint *point = map_path.first->point;

		const int32_t target_x_i = point->x;
		const int32_t target_y_i = point->y;

		const double target_x_d = target_x_i;
		const double target_y_d = target_y_i;



		Track *leftTrack = tank->leftTrack;
		Track *rightTrack = tank->rightTrack;


		/*
		if (stop_motion_2 == true)
		{
			if (speed > 300)
			{
				//leftTrack->bot_turn_add_value = 0.;
				//rightTrack->bot_turn_add_value = 0.;
				tankControl->stop_motion();
			}
			else
			{
				stop_motion_2 = false;
			}
		}
		else
		{
			if (stop_2 == true && speed > 400)
			{
				//leftTrack->bot_turn_add_value = 0.;
				//rightTrack->bot_turn_add_value = 0.;
				tankControl->stop_motion();
				stop_motion_2 = true;
				//return;
			}
		}
		*/




		if (stop_motion_1 == true)
		{
			if (speed > 800.)
			{
				tankControl->stop_motion();
			}
			else
			{
				stop_motion_1 = false;
			}
		}
		else
		{
			double distance = BotUtils::distance_d_x_y(hullMesh->position.x, hullMesh->position.y, target_x_d, target_y_d);
			double v = speed * 0.8;

			if (speed > 900. && v > distance)
			{
				uint8_t lenght = 10;

				bool stop = false;

				double radian = BotUtils::get_radian(target_x_i, target_y_i, my_x, my_y);

				if (radian > 0.4065) {
					if (radian < 1.0874) {
						stop = !find_xm_yp(point, lenght);
					} else if (radian < 2.0093) {
						stop = !find_xm(point, lenght);
					} else if (radian < 2.7020) {
						stop = !find_xm_ym(point, lenght);
					} else if (radian < 3.6476) {
						stop = !find_ym(point, lenght);
					} else if (radian < 4.2487) {
						stop = !find_xp_ym(point, lenght);
					} else if (radian < 5.1907) {
						stop = !find_xp(point, lenght);
					} else if (radian < 5.7979) {
						stop = !find_xp_yp(point, lenght);
					} else {
						stop = !find_yp(point, lenght);
					}
				} else {
					stop = !find_yp(point->YP, lenght);
				}


				if (stop == true)
				{
					tankControl->stop_motion();
					stop_motion_1 = true;
				}
			}
		}

		//bool stop_motion = stop_motion_1 | stop_motion_2;
		bool stop_motion = stop_motion_1;

		if (stop_motion == true)
		{
			tankControl->stop_motion();
			leftTrack->bot_turn_add_value = 0.;
			rightTrack->bot_turn_add_value = 0.;
			return true;
		}

		//return;


		//__Vector3 angles;
		//Quaternion_getEulerAngles(tank->state->orientation, &angles);
		//const double angles_z = Quaternion_getEulerAngleZ(tank->state->orientation);


		const double v1 = atan2(target_x_d - hullMesh->position.x, target_y_d - hullMesh->position.y);
		const double angles_z = hullMesh->rotation.z;


		{
			double d = BotUtils::difference(angles_z, -v1);
			double c = MyMath::abs(d);

			if (c < 1.57)
			{
				bool c1 = c > 0.01; //0.1
				bool c2 = c > 0.1;
				bool c3 = c > 0.3;

				double g = c * 15.0;

				if (c2 == true) {
					tankControl->stop_motion();
				} else if (stop_motion == false) {
					tankControl->forward();
				}


				if (c1 == true)
				{
					if (d > 0.)
					{
						if (c3 == false && tank->state.rotation.z < (tankControl->is_right() ? -0.9 : -0.1))
						{
							tankControl->stop_turn();
						}
						else
						{
							if (c2 == true)
							{
								tankControl->right();
								leftTrack->bot_turn_add_value = 0.;
								rightTrack->bot_turn_add_value = 0.;
							}
							else
							{
								tankControl->stop_turn();
								leftTrack->bot_turn_add_value  = 123000 * g;
								rightTrack->bot_turn_add_value = -123000 * g;
							}
						}
					}
					else
					{
						if (c3 == false && tank->state.rotation.z > (tankControl->is_left() ? 0.9 : 0.1))
						{
							tankControl->stop_turn();
						}
						else
						{
							if (c2 == true)
							{
								tankControl->left();
								leftTrack->bot_turn_add_value = 0.;
								rightTrack->bot_turn_add_value = 0.;
							}
							else
							{
								tankControl->stop_turn();
								leftTrack->bot_turn_add_value = -123000 * g;
								rightTrack->bot_turn_add_value = 123000 * g;
							}
						}
					}
				}


				if (c1 == false)
				{
					/*
					angles.z = -v1;
					Quaternion_setFromEulerAnglesXYZ(tank->state->orientation, angles.x, angles.y, angles.z);
					Quaternion_copy(tank->prevState->orientation, tank->state->orientation);
					*/

					leftTrack->bot_turn_add_value = 0.;
					rightTrack->bot_turn_add_value = 0.;
					tankControl->stop_turn();
				}
			}
			else
			{
				if (c > M_PI)
					c = 0.;
				c = M_PI - c;

				bool c1 = c > 0.01;
				bool c2 = c > 0.1;
				bool c3 = c > 0.3;

				double g = c * 15.0;

				if (c2 == true) {
					tankControl->stop_motion();
				} else if (stop_motion == false) {
					tankControl->back();
				}

				if (c1 == true)
				{
					if (d > 0.)
					{
						if (c3 == false && tank->state.rotation.z > (tankControl->is_left() ? 0.9 : 0.1))
						{
							tankControl->stop_turn();
						}
						else
						{
							if (c2 == true)
							{
								tankControl->left();
								leftTrack->bot_turn_add_value = 0.;
								rightTrack->bot_turn_add_value = 0.;
							}
							else
							{
								tankControl->stop_turn();
								leftTrack->bot_turn_add_value = -123000 * g;
								rightTrack->bot_turn_add_value = 123000 * g;
							}
						}

					}
					else
					{
						if (c3 == false && tank->state.rotation.z < (tankControl->is_right() ? -0.9 : -0.1))
						{
							tankControl->stop_turn();
						}
						else
						{
							if (c2 == true)
							{
								tankControl->right();
								leftTrack->bot_turn_add_value = 0.;
								rightTrack->bot_turn_add_value = 0.;
							}
							else
							{
								tankControl->stop_turn();
								leftTrack->bot_turn_add_value = 123000 * g;
								rightTrack->bot_turn_add_value = -123000 * g;
							}
						}
					}
				}
				else
				{
					//angles.z = BotUtils::direction_normalize((-v1) + 3.14);
					//Quaternion_setFromEulerAnglesXYZ(tank->state->orientation, angles.x, angles.y, angles.z);
					//Quaternion_copy(tank->prevState->orientation, tank->state->orientation);

					leftTrack->bot_turn_add_value = 0.;
					rightTrack->bot_turn_add_value = 0.;
					tankControl->stop_turn();
				}
			}
		}
		return false;
	}

};



Bot::Bot(TankModel *tankModel, TANKS::StructTank *structTank, BotSettings *botSettings, TanksCollisionDetector *collisionDetector) :
	d(new BotPrivate(tankModel, structTank, botSettings, collisionDetector))
{

}

Bot::~Bot()
{
	delete d;
}

void Bot::run(uint32_t time)
{
	if (d->enable == false)
		return;
	d->run(time);
}

void Bot::deleteTank(Tank *tank)
{
	d->deleteTank(tank);
}

MapPoints* Bot::getMapPoints()
{
	return &d->map;
}

MapListPath* Bot::getMapListPath()
{
	return &d->map_path;
}

void Bot::spawnTank(uint32_t time)
{
	d->spawnTank(time);
}

BotSettings* Bot::getBotSettings()
{
	return d->botSettings;
}

void Bot::savePoints()
{
	d->savePoints();
}

void Bot::loadPoints()
{
	d->loadPoints();
}

void Bot::setEnable(bool status)
{
	d->setEnable(status);
}

void Bot::updateCommands()
{
	d->updateCommands();
}
