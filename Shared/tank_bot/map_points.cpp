#include "map_points.h"
#include "astar.h"
#include "tanks.h"
#include "map_list_path.h"
#include "bot_utils.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/tanks/physics/collisiongroup.h"
//#include <math.h>
#include <QVector>




void map_point_clear();
static bool map_point_init_1(MapPoints *map);
static void map_point_init_2(MapPoints *map);
static void map_point_init_3(MapPoints *map);
static void map_point_init_4(MapPoints *map);
static void map_point_init_5(MapPoints *map);
static void map_point_init_6(MapPoints *map);


#define DISTANCE_1 120.0
#define DISTANCE_2 60.01
#define DISTANCE_3 150.0
#define BLOCK_SIZE 60


namespace
{
	struct VectorInt3
	{
		int32_t x;
		int32_t y;
		int32_t z;
	};
}

enum
{
	DIAGONAL_XP_YP = 1,
	DIAGONAL_XP_YM = 2,
	DIAGONAL_XM_YP = 4,
	DIAGONAL_XM_YM = 8,
};








static const Vector3 dir_down(0, 0, -1);
static const Vector3 dir_up(0, 0, 1);
static const Vector3 dir_XP(1, 0, 0);
static const Vector3 dir_XM(-1, 0, 0);
static const Vector3 dir_YP(0, 1, 0);
static const Vector3 dir_YM(0, -1, 0);
static const Vector3 dir_xp_yp(0.707106781186547462, 0.707106781186547462, 0);
static const Vector3 dir_xp_ym(0.707106781186547462, -0.707106781186547462, 0);
static const Vector3 dir_xm_yp(-0.707106781186547462, 0.707106781186547462, 0);
static const Vector3 dir_xm_ym(-0.707106781186547462, -0.707106781186547462, 0);




static inline bool IntersectRay(TanksCollisionDetector *_this,
								const Vector3 *origin,
								const Vector3 *dir,
								double maxTime,
								RayIntersection *result)
{
	return _this->intersectRayWithStatic(origin, dir, CollisionGroup::STATIC, maxTime, nullptr, result);
}



static inline void Vector3_dir(int32_t ax, int32_t ay, int32_t az, int32_t bx, int32_t by, int32_t bz, Vector3 *dir)
{
	int32_t int_x = bx - ax;
	int32_t int_y = by - ay;
	int32_t int_z = bz - az;

	int32_t d = (int_x * int_x) + (int_y * int_y) + (int_z * int_z);
	if (d == 0)
	{
		dir->x = 1.;
		dir->y = 0.;
		dir->z = 0.;
	}
	else
	{
		double v = 1. / sqrt(d);
		dir->x = int_x * v;
		dir->y = int_y * v;
		dir->z = int_z * v;
	}
}


static inline MapPoint* map_array_get(MapPoints *map, uint32_t id)
{
	id -= 1;
	if (map->length <= id)
		return nullptr;
	return &map->point_list[id];
}




static bool IntersectRay_down(TanksCollisionDetector *_this, VectorInt3 *origin_int, double &distance, CollisionPrimitive *&collision)
{
	Vector3 origin_1;
	origin_1.x = origin_int->x;
	origin_1.y = origin_int->y;
	origin_1.z = origin_int->z;

	RayIntersection result;
	if (IntersectRay(_this, &origin_1, &dir_down, DISTANCE_1, &result) == true) {
		distance  = result.t;
		collision = result.primitive;
		return true;
	}

	return false;
}


static bool IntersectRay_up(TanksCollisionDetector *_this, VectorInt3 *origin_int)
{
	Vector3 origin_1;
	origin_1.x = origin_int->x;
	origin_1.y = origin_int->y;
	origin_1.z = origin_int->z;

	RayIntersection result;
	if (IntersectRay(_this, &origin_1, &dir_up, DISTANCE_3, &result) == true) {
		return true;
	}

	return false;
}




static bool IntersectRay_side_xp(TanksCollisionDetector *_this, VectorInt3 *origin_int)
{
	Vector3 origin_1;
	origin_1.x = origin_int->x;
	origin_1.y = origin_int->y;
	origin_1.z = origin_int->z;

	RayIntersection result;
	if (IntersectRay(_this, &origin_1, &dir_XP, DISTANCE_2, &result) == true)
		return true;

	return false;
}


static bool IntersectRay_side_xm(TanksCollisionDetector *_this, VectorInt3 *origin_int)
{
	Vector3 origin_1;
	origin_1.x = origin_int->x;
	origin_1.y = origin_int->y;
	origin_1.z = origin_int->z;

	RayIntersection result;
	if (IntersectRay(_this, &origin_1, &dir_XM, DISTANCE_2, &result) == true)
		return true;

	return false;
}


static bool IntersectRay_side_yp(TanksCollisionDetector *_this, VectorInt3 *origin_int)
{
	Vector3 origin_1;
	origin_1.x = origin_int->x;
	origin_1.y = origin_int->y;
	origin_1.z = origin_int->z;

	RayIntersection result;
	if (IntersectRay(_this, &origin_1, &dir_YP, DISTANCE_2, &result) == true)
		return true;

	return false;
}


static bool IntersectRay_side_ym(TanksCollisionDetector *_this, VectorInt3 *origin_int)
{
	Vector3 origin_1;
	origin_1.x = origin_int->x;
	origin_1.y = origin_int->y;
	origin_1.z = origin_int->z;

	RayIntersection result;
	if (IntersectRay(_this, &origin_1, &dir_YM, DISTANCE_2, &result) == true)
		return true;

	return false;
}


static void IntersectRay_side(TanksCollisionDetector *_this, VectorInt3 *origin_int, MapPoint *point)
{
	if (IntersectRay_side_xp(_this, origin_int) == true) {
		point->block_xp = true;
	}

	if (IntersectRay_side_xm(_this, origin_int) == true) {
		point->block_xm = true;
	}

	if (IntersectRay_side_yp(_this, origin_int) == true) {
		point->block_yp = true;
	}

	if (IntersectRay_side_ym(_this, origin_int) == true) {
		point->block_ym = true;
	}
}


static bool IntersectRay_side_diagonal(TanksCollisionDetector *_this, const MapPoint *point_a, const MapPoint *point_b, const Vector3 *dir)
{
	double distance;

	Vector3 dir_temp;

	if (point_a->z == point_b->z)
	{
		distance = 84.8528; //120;
		//0.707106781186547462
	}
	else
	{
		distance = BotUtils::distance_d_x_y_z(point_a->x, point_a->y, point_a->z, point_b->x, point_b->y, point_b->z);
		Vector3_dir(point_a->x, point_a->y, point_a->z, point_b->x, point_b->y, point_b->z, &dir_temp);
		dir = &dir_temp;
	}

	Vector3 origin_1;
	origin_1.x = point_a->x;
	origin_1.y = point_a->y;
	origin_1.z = point_a->z;

	RayIntersection result;
	return IntersectRay(_this, &origin_1, dir, distance, &result);
}


static bool IntersectRay_side_z(TanksCollisionDetector *_this, const MapPoint *point_a, const MapPoint *point_b, uint32_t mz)
{
	int32_t mza = point_a->z - mz;
	int32_t mzb = point_b->z - mz;

	double distance = BotUtils::distance_d_x_y_z(point_a->x, point_a->y, mza, point_b->x, point_b->y, mzb);

	Vector3 dir;
	Vector3_dir(point_a->x, point_a->y, mza, point_b->x, point_b->y, mzb, &dir);

	Vector3 origin_1;
	origin_1.x = point_a->x;
	origin_1.y = point_a->y;
	origin_1.z = mza;

	RayIntersection result;
	return IntersectRay(_this, &origin_1, &dir, distance, &result);
}


static bool IntersectRay_side_z(TanksCollisionDetector *_this, const MapPoint *point_a, const VectorInt3 *pos_b, uint32_t mz)
{
	int32_t mza = point_a->z - mz;
	int32_t mzb = pos_b->z - mz;

	double distance = BotUtils::distance_d_x_y_z(point_a->x, point_a->y, mza, pos_b->x, pos_b->y, mzb);

	Vector3 dir;
	Vector3_dir(point_a->x, point_a->y, mza, pos_b->x, pos_b->y, mzb, &dir);

	Vector3 origin_1;
	origin_1.x = point_a->x;
	origin_1.y = point_a->y;
	origin_1.z = mza;

	RayIntersection result;
	return IntersectRay(_this, &origin_1, &dir, distance, &result);
}


static bool IntersectRay_side_down(TanksCollisionDetector *_this, const MapPoint *point, const Vector3 *dir, uint32_t mz)
{
	Vector3 origin_1;
	origin_1.x = point->x;
	origin_1.y = point->y;
	origin_1.z = point->z - mz;

	RayIntersection result;
	if (IntersectRay(_this, &origin_1, dir, DISTANCE_2, &result) == true)
		return true;

	return false;
}


static bool IntersectRay_down_z(TanksCollisionDetector *_this, const MapPoint *point_a, const MapPoint *point_b, const Vector3 *dir)
{
	if (point_a->z != point_b->z)
	{
		return IntersectRay_side_z(_this, point_a, point_b, 45);
	}
	else
	{
		return IntersectRay_side_down(_this, point_a, dir, 55);
	}
}


static bool IntersectRay_down_z(TanksCollisionDetector *_this, const MapPoint *point_a, const VectorInt3 *pos_b, const Vector3 *dir)
{
	if (point_a->z != pos_b->z)
	{
		return IntersectRay_side_z(_this, point_a, pos_b, 45);
	}
	else
	{
		return IntersectRay_side_down(_this, point_a, dir, 55);
	}
}




namespace
{
	struct FPoint
	{
		FPoint *next;
		MapPoint *point;

		FPoint(MapPoint *point)
		{
			this->next = nullptr;
			this->point = point;
		}
	};

	struct F2
	{
		int32_t y;
		F2 *next;
		FPoint *first_fpoint;
		FPoint *end_fpoint;

		F2(int32_t y)
		{
			this->y = y;
			this->next = nullptr;
			this->first_fpoint = nullptr;
			this->end_fpoint = nullptr;
		}

		~F2()
		{
			FPoint *current = first_fpoint;
			while (current != nullptr)
			{
				FPoint *next = current->next;
				delete current;
				current = next;
			}
		}

		void addFPoint(FPoint *point)
		{
			if (first_fpoint == nullptr)
				first_fpoint = point;
			if (end_fpoint != nullptr)
				end_fpoint->next = point;
			end_fpoint = point;
		}
	};


	struct F1
	{
		int32_t x;
		F1 *next;
		F2 *first_f2;
		F2 *end_f2;

		F1(int32_t x)
		{
			this->x = x;
			this->next = nullptr;
			this->first_f2 = nullptr;
			this->end_f2 = nullptr;
		}

		~F1()
		{
			F2 *current = first_f2;
			while (current != nullptr)
			{
				F2 *next = current->next;
				delete current;
				current = next;
			}
		}

		void addF2(F2 *f2)
		{
			if (first_f2 == nullptr)
				first_f2 = f2;
			if (end_f2 != nullptr)
				end_f2->next = f2;
			end_f2 = f2;
		}
	};


	struct FF
	{
		F1 *first_f1;
		F1 *end_f1;

		FF()
		{
			first_f1 = nullptr;
			end_f1 = nullptr;
		}

		~FF()
		{
			F1 *current = first_f1;
			while (current != nullptr)
			{
				F1 *next = current->next;
				delete current;
				current = next;
			}
		}

		void addF1(F1 *f1)
		{
			if (first_f1 == nullptr)
				first_f1 = f1;
			if (end_f1 != nullptr)
				end_f1->next = f1;
			end_f1 = f1;
		}
	};


	MapPoint* point_find(FF *ff, int32_t x, int32_t y, int32_t z)
	{
		F1 *f1_current = ff->first_f1;
		while (f1_current != nullptr)
		{
			if (f1_current->x == x)
			{
				F2 *f2_current = f1_current->first_f2;

				while (f2_current != nullptr)
				{
					if (f2_current->y == y)
					{
						FPoint *fpoint_current = f2_current->first_fpoint;
						while (fpoint_current != nullptr)
						{
							if (abs(fpoint_current->point->z - z) < 100)
								return fpoint_current->point;

							fpoint_current = fpoint_current->next;
						}

						return nullptr;
					}

					f2_current = f2_current->next;
				}

				return nullptr;
			}

			f1_current = f1_current->next;
		}

		return nullptr;
	}


	void point_add(FF *ff, MapPoint *point)
	{
		int32_t x = point->x;
		int32_t y = point->y;

		FPoint *fpoint = new FPoint(point);

		F1 *f1_current = ff->first_f1;
		while (f1_current != nullptr)
		{
			if (f1_current->x == x)
			{
				F2 *f2_current = f1_current->first_f2;
				while (f2_current != nullptr)
				{
					if (f2_current->y == y)
					{
						f2_current->addFPoint(fpoint);
						return;
					}

					f2_current = f2_current->next;
				}

				F2 *f2 = new F2(y);
				f2->addFPoint(fpoint);
				f1_current->addF2(f2);
				return;
			}

			f1_current = f1_current->next;
		}

		F2 *new_f2 = new F2(fpoint->point->y);
		new_f2->addFPoint(fpoint);
		F1 *new_f1 = new F1(fpoint->point->x);
		new_f1->addF2(new_f2);
		ff->addF1(new_f1);
	}
}



void MapPoint::reset()
{
	this->INDEX_XP = 0;
	this->INDEX_XM = 0;
	this->INDEX_YP = 0;
	this->INDEX_YM = 0;
	this->block_xp = false;
	this->block_xm = false;
	this->block_yp = false;
	this->block_ym = false;
}


void MapPoints::create(double x, double y, double z)
{
	clear();


	FF ff;

	enum class PointDir { XP, XM, YP, YM };


	QVector<MapPoint*> list;

	uint32_t index = 0;


	MapPoint *point_current = new MapPoint;
	list.append(point_current);

	point_current->prev = nullptr;
	point_current->next = nullptr;
	point_current->INDEX = ++index;

	point_current->reset();

	point_first = point_current;
	point_end = point_current;

	VectorInt3 current_pos;
	current_pos.x = x;
	current_pos.y = y;
	current_pos.z = z;

	Vector3 origin_1;
	origin_1.x = current_pos.x;
	origin_1.y = current_pos.y;
	origin_1.z = current_pos.z;

	RayIntersection result;

	if (IntersectRay(this->collisionDetector, &origin_1, &dir_down, 10000, &result) == false)
		return;

	current_pos.z = (current_pos.z - result.t) + 59;

	point_current->x = current_pos.x;
	point_current->y = current_pos.y;
	point_current->z = current_pos.z;

	point_add(&ff, point_current);

	PointDir point_dir = PointDir::XP;

	double distance;

	for (;;)
	{
		for (;;)
		{
			if (point_current->block_xp == false && point_current->INDEX_XP == 0) {
				point_dir = PointDir::XP;
				current_pos.x += BLOCK_SIZE;
			} else if (point_current->block_yp == false && point_current->INDEX_YP == 0) {
				point_dir = PointDir::YP;
				current_pos.y += BLOCK_SIZE;
			} else if (point_current->block_xm == false && point_current->INDEX_XM == 0) {
				point_dir = PointDir::XM;
				current_pos.x -= BLOCK_SIZE;
			} else if (point_current->block_ym == false && point_current->INDEX_YM == 0) {
				point_dir = PointDir::YM;
				current_pos.y -= BLOCK_SIZE;
			} else {
				if (list.isEmpty())
					break;

				point_current = list.takeLast();
				current_pos.x = point_current->x;
				current_pos.y = point_current->y;
				current_pos.z = point_current->z;
				continue;
			}

			//MapPoint *found = find(current_pos->x, current_pos->y, current_pos->z);
			MapPoint *found = point_find(&ff, current_pos.x, current_pos.y, current_pos.z);

			if (found != nullptr)
			{
				switch (point_dir)
				{
					case PointDir::XP:
						current_pos.x -= BLOCK_SIZE;
						if (found->block_xm == true)
						{
							point_current->block_xp = true;
						}
						else
						{
							if (IntersectRay_down_z(this->collisionDetector, point_current, found, &dir_XP) == true)
							{
								point_current->block_xp = true;
								found->block_xm = true;
							}
							else
							{
								point_current->INDEX_XP = found->INDEX;
								found->INDEX_XM = point_current->INDEX;
							}
						}
						break;
					case PointDir::XM:
						current_pos.x += BLOCK_SIZE;
						if (found->block_xp == true)
						{
							point_current->block_xm = true;
						}
						else
						{
							if (IntersectRay_down_z(this->collisionDetector, point_current, found, &dir_XM) == true)
							{
								point_current->block_xm = true;
								found->block_xp = true;
							}
							else
							{
								point_current->INDEX_XM = found->INDEX;
								found->INDEX_XP = point_current->INDEX;
							}
						}
						break;
					case PointDir::YP:
						current_pos.y -= BLOCK_SIZE;
						if (found->block_ym == true)
						{
							point_current->block_yp = true;
						}
						else
						{
							if (IntersectRay_down_z(this->collisionDetector, point_current, found, &dir_YP) == true)
							{
								point_current->block_yp = true;
								found->block_ym = true;
							}
							else
							{
								point_current->INDEX_YP = found->INDEX;
								found->INDEX_YM = point_current->INDEX;
							}
						}
						break;
					case PointDir::YM:
						current_pos.y += BLOCK_SIZE;
						if (found->block_yp == true)
						{
							point_current->block_ym = true;
						}
						else
						{
							if (IntersectRay_down_z(this->collisionDetector, point_current, found, &dir_YM) == true)
							{
								point_current->block_ym = true;
								found->block_yp = true;
							}
							else
							{
								point_current->INDEX_YM = found->INDEX;
								found->INDEX_YP = point_current->INDEX;
							}
						}
						break;
				}
			}
			else
			{
				break;
			}
		}

		if (list.isEmpty())
			break;



		CollisionPrimitive *collision;


		bool block = IntersectRay_down(this->collisionDetector, &current_pos, distance, collision) == false;
		if (block == false)
			block = IntersectRay_up(this->collisionDetector, &current_pos) == true;

		if (block == false)
		{
			current_pos.z = (current_pos.z - distance) + 59;

			switch (point_dir)
			{
				case PointDir::XP:
					if (IntersectRay_down_z(this->collisionDetector, point_current, &current_pos, &dir_XP) == true)
						block = true;
					break;
				case PointDir::XM:
					if (IntersectRay_down_z(this->collisionDetector, point_current, &current_pos, &dir_XM) == true)
						block = true;
					break;
				case PointDir::YP:
					if (IntersectRay_down_z(this->collisionDetector, point_current, &current_pos, &dir_YP) == true)
						block = true;
					break;
				case PointDir::YM:
					if (IntersectRay_down_z(this->collisionDetector, point_current, &current_pos, &dir_YM) == true)
						block = true;
					break;
			}
		}


		if (block == false)
		{	
			MapPoint *point_new = new MapPoint;
			point_new->INDEX = ++index;
			point_new->prev = point_end;
			point_new->next = nullptr;
			point_new->reset();
			point_end->next = point_new;
			point_end = point_new;


			/*
			static int g;
			if (++g == 500) {
				g = 0;
				qDebug() << point_new->INDEX;
			}
			*/

			point_new->x = current_pos.x;
			point_new->y = current_pos.y;
			point_new->z = current_pos.z;

			IntersectRay_side(this->collisionDetector, &current_pos, point_new);

			switch (point_dir)
			{
				case PointDir::XP:
					point_current->INDEX_XP = point_new->INDEX;
					point_new->INDEX_XM = point_current->INDEX;
					break;
				case PointDir::XM:
					point_current->INDEX_XM = point_new->INDEX;
					point_new->INDEX_XP = point_current->INDEX;
					break;
				case PointDir::YP:
					point_current->INDEX_YP = point_new->INDEX;
					point_new->INDEX_YM = point_current->INDEX;
					break;
				case PointDir::YM:
					point_current->INDEX_YM = point_new->INDEX;
					point_new->INDEX_YP = point_current->INDEX;
					break;
			}

			list.append(point_new);

			point_add(&ff, point_new);

			point_current = point_new;
		}

		if (block == true)
		{
			switch (point_dir)
			{
				case PointDir::XP:
					current_pos.x -= BLOCK_SIZE;
					point_current->block_xp = true;
					break;
				case PointDir::XM:
					current_pos.x += BLOCK_SIZE;
					point_current->block_xm = true;
					break;
				case PointDir::YP:
					current_pos.y -= BLOCK_SIZE;
					point_current->block_yp = true;
					break;
				case PointDir::YM:
					current_pos.y += BLOCK_SIZE;
					point_current->block_ym = true;
					break;
			}
		}
	}




	this->length = 0;
	MapPoint *point = this->point_first;
	while (point != nullptr) {
		this->length++;
		point = point->next;
	}

	this->point_list = new MapPoint[this->length];
	this->point_list[0].prev = nullptr;

	MapPoint *parent = nullptr;
	index = 0;
	point = this->point_first;
	while (point != nullptr)
	{
		MapPoint *p = &this->point_list[index];
		p->block_xp = point->block_xp;
		p->block_xm = point->block_xm;
		p->block_yp = point->block_yp;
		p->block_ym = point->block_ym;

		p->INDEX = point->INDEX;
		p->INDEX_XP = point->INDEX_XP;
		p->INDEX_XM = point->INDEX_XM;
		p->INDEX_YP = point->INDEX_YP;
		p->INDEX_YM = point->INDEX_YM;

		p->x = point->x;
		p->y = point->y;
		p->z = point->z;

		p->next = nullptr;
		p->prev = parent;

		p->diagonal = 0;

		if (parent != nullptr)
			parent->next = p;

		parent = p;

		this->point_end = p;
		index++;
		MapPoint *next = point->next;
		delete point;
		point = next;
	}

	this->point_first = this->point_list;

	map_point_init_1(this);
	map_point_init_2(this);
	map_point_init_3(this);
	map_point_init_4(this);


	point = this->point_first;
	while (point != nullptr)
	{
		if (point->XP_YP != nullptr)
		{
			if (point->block_xp || point->block_yp || IntersectRay_side_diagonal(this->collisionDetector, point, point->XP_YP, &dir_xp_yp))
			{
				point->XP_YP = nullptr;
			}
			else
			{
				point->diagonal |= DIAGONAL_XP_YP;
			}
		}

		if (point->XP_YM != nullptr)
		{
			if (point->block_xp || point->block_ym || IntersectRay_side_diagonal(this->collisionDetector, point, point->XP_YM, &dir_xp_ym))
			{
				point->XP_YM = nullptr;
			}
			else
			{
				point->diagonal |= DIAGONAL_XP_YM;
			}
		}

		if (point->XM_YP != nullptr)
		{
			if (point->block_xm || point->block_yp || IntersectRay_side_diagonal(this->collisionDetector, point, point->XM_YP, &dir_xm_yp))
			{
				point->XM_YP = nullptr;
			}
			else
			{
				point->diagonal |= DIAGONAL_XM_YP;
			}
		}

		if (point->XM_YM != nullptr)
		{
			if (point->block_xm || point->block_ym || IntersectRay_side_diagonal(this->collisionDetector, point, point->XM_YM, &dir_xm_ym))
			{
				point->XM_YM = nullptr;
			}
			else
			{
				point->diagonal |= DIAGONAL_XM_YM;
			}
		}

		point = point->next;
	}

	point = this->point_first;
	while (point != nullptr)
	{
		if (point->XP_YP != nullptr)
		{
			if (point->XP_YP->XM_YM == nullptr) {
				point->XP_YP = nullptr;
				point->diagonal &= ~DIAGONAL_XP_YP;
			}
		}

		if (point->XP_YM != nullptr)
		{
			if (point->XP_YM->XM_YP == nullptr) {
				point->XP_YM = nullptr;
				point->diagonal &= ~DIAGONAL_XP_YM;
			}
		}

		if (point->XM_YP != nullptr)
		{
			if (point->XM_YP->XP_YM == nullptr) {
				point->XM_YP = nullptr;
				point->diagonal &= ~DIAGONAL_XM_YP;
			}
		}

		if (point->XM_YM != nullptr)
		{
			if (point->XM_YM->XP_YP == nullptr) {
				point->XM_YM = nullptr;
				point->diagonal &= ~DIAGONAL_XM_YM;
			}
		}

		point = point->next;
	}


	map_point_init_5(this);
}


MapPoint::MapPoint()
{
	XP = nullptr;
	XM = nullptr;
	YP = nullptr;
	YM = nullptr;
	XP_YP = nullptr;
	XP_YM = nullptr;
	XM_YP = nullptr;
	XM_YM = nullptr;
	next = nullptr;
	prev = nullptr;
	block_xp = false;
	block_xm = false;
	block_yp = false;
	block_ym = false;
	locked = false;
	diagonal = 0;

	astar_next_free = nullptr;
	astar_parent = nullptr;
	astar_next = nullptr;
	astar_Gcost = 0;
	astar_Fcost = 0;
	astar_Hcost = 0;
	astar_status = EMPTY;
	astar_add_free = false;
}


static bool map_point_init_1(MapPoints *map)
{
	MapPoint *point = map->point_first;

	while (point != nullptr)
	{
		if (point->INDEX_XP != 0) {
			point->XP = map_array_get(map, point->INDEX_XP);
			if (point->XP == nullptr) {
				return false;
			}
		} else {
			point->XP = nullptr;
		}

		if (point->INDEX_XM != 0) {
			point->XM = map_array_get(map, point->INDEX_XM);
			if (point->XM == nullptr) {
				return false;
			}
		} else {
			point->XM = nullptr;
		}

		if (point->INDEX_YP != 0) {
			point->YP = map_array_get(map, point->INDEX_YP);
			if (point->YP == nullptr) {
				return false;
			}
		} else {
			point->YP = nullptr;
		}

		if (point->INDEX_YM != 0) {
			point->YM = map_array_get(map, point->INDEX_YM);
			if (point->YM == nullptr) {
				return false;
			}
		} else {
			point->YM = nullptr;
		}

		point = point->next;
	}

	return true;
}


static void map_point_init_2(MapPoints *map)
{
	MapPoint *point = map->point_first;

	while (point != nullptr)
	{
		if (point->XP != nullptr && point->XP->YP != nullptr) {
			point->XP_YP = point->XP->YP;
		} else if (point->YP != nullptr && point->YP->XP != nullptr) {
			point->XP_YP = point->YP->XP;
		}

		if (point->XM != nullptr && point->XM->YP != nullptr) {
			point->XM_YP = point->XM->YP;
		} else if (point->YP != nullptr && point->YP->XM != nullptr) {
			point->XM_YP = point->YP->XM;
		}

		if (point->XM != nullptr && point->XM->YM != nullptr) {
			point->XM_YM = point->XM->YM;
		} else if (point->YM != nullptr && point->YM->XM != nullptr) {
			point->XM_YM = point->YM->XM;
		}

		if (point->XP != nullptr && point->XP->YM != nullptr) {
			point->XP_YM = point->XP->YM;
		} else if (point->YM != nullptr && point->YM->XP != nullptr) {
			point->XP_YM = point->YM->XP;
		}

		point = point->next;
	}
}


static void map_point_init_3(MapPoints *map)
{
	MapPoint *point = map->point_first;
	point->_x = 0;
	point->_y = 0;
	point->_z = 0;

	while (point != nullptr)
	{
		MapPoint *next = point->next;
		if (next == nullptr)
			break;

		while (point != nullptr)
		{
			if (point->XP == next) {
				next->_x = point->_x + 1;
				next->_y = point->_y;
				break;
			} else if (point->XM == next) {
				next->_x = point->_x - 1;
				next->_y = point->_y;
				break;
			} else if (point->YP == next) {
				next->_y = point->_y + 1;
				next->_x = point->_x;
				break;
			} else if (point->YM == next) {
				next->_y = point->_y - 1;
				next->_x = point->_x;
				break;
			} else {
				point = point->prev;
				continue;
			}
		}

		point = next;
	}
}


static void map_point_init_4(MapPoints *map)
{
	MapPoint *point = map->point_first;
	int32_t c = point->z;
	while (point != nullptr)
	{
		int32_t z = point->z - c;
		point->_z = z / BLOCK_SIZE;
		point = point->next;
	}
}


static inline bool check_XP(MapPoint *point)
{
	for (uint8_t i = 0; i < 6; i++) {
		point = point->XP;
		if (point == nullptr)
			return true;
	}
	return false;
}


static inline bool check_YP(MapPoint *point)
{
	for (uint8_t i = 0; i < 6; i++) {
		point = point->YP;
		if (point == nullptr)
			return true;
	}
	return false;
}



static bool check(MapPoint *point)
{
	point = point->XM_YM;
	if (point == nullptr)
		return false;
	point = point->XM_YM;
	if (point == nullptr)
		return false;
	point = point->XM_YM;
	if (point == nullptr)
		return false;

	MapPoint *X1 = point;
	MapPoint *Y1 = point;

	MapPoint *X2 = X1->XP;
	if (X2 == nullptr)
		return false;

	MapPoint *X3 = X2->XP;
	if (X3 == nullptr)
		return false;

	MapPoint *X4 = X3->XP;
	if (X4 == nullptr)
		return false;

	MapPoint *X5 = X4->XP;
	if (X5 == nullptr)
		return false;

	MapPoint *X6 = X5->XP;
	if (X6 == nullptr)
		return false;

	MapPoint *X7 = X6->XP;
	if (X7 == nullptr)
		return false;

	MapPoint *Y2 = Y1->YP;
	if (Y2 == nullptr)
		return false;

	MapPoint *Y3 = Y2->YP;
	if (Y3 == nullptr)
		return false;

	MapPoint *Y4 = Y3->YP;
	if (Y4 == nullptr)
		return false;

	MapPoint *Y5 = Y4->YP;
	if (Y5 == nullptr)
		return false;

	MapPoint *Y6 = Y5->YP;
	if (Y6 == nullptr)
		return false;

	MapPoint *Y7 = Y6->YP;
	if (Y7 == nullptr)
		return false;

	if (check_YP(X1) == true)
		return false;

	if (check_YP(X2) == true)
		return false;

	if (check_YP(X3) == true)
		return false;

	if (check_YP(X4) == true)
		return false;

	if (check_YP(X5) == true)
		return false;

	if (check_YP(X6) == true)
		return false;

	if (check_YP(X7) == true)
		return false;

	if (check_XP(Y1) == true)
		return false;

	if (check_XP(Y2) == true)
		return false;

	if (check_XP(Y3) == true)
		return false;

	if (check_XP(Y4) == true)
		return false;

	if (check_XP(Y5) == true)
		return false;

	if (check_XP(Y6) == true)
		return false;

	return check_XP(Y7) == false;
}


static void map_point_init_5(MapPoints *map)
{
	MapPoint *point = map->point_first;
	while (point != nullptr)
	{
		point->locked = check(point) == false;
		point = point->next;
	}
}


static void map_point_init_6(MapPoints *map)
{
	MapPoint *point = map->point_first;

	while (point != nullptr)
	{
		uint8_t diagonal = point->diagonal;

		if (point->XP_YP != nullptr)
		{
			if ((diagonal & DIAGONAL_XP_YP) == 0)
			{
				point->XP_YP->XM_YM = nullptr;
				point->XP_YP = nullptr;
			}
		}

		if (point->XP_YM != nullptr)
		{
			if ((diagonal & DIAGONAL_XP_YM) == 0) {

				point->XP_YM->XM_YP = nullptr;
				point->XP_YM = nullptr;
			}
		}

		if (point->XM_YP != nullptr)
		{
			if ((diagonal & DIAGONAL_XM_YP) == 0)
			{
				point->XM_YP->XP_YM = nullptr;
				point->XM_YP = nullptr;
			}
		}

		if (point->XM_YM != nullptr)
		{
			if ((diagonal & DIAGONAL_XM_YM) == 0)
			{
				point->XM_YM->XP_YP = nullptr;
				point->XM_YM = nullptr;
			}
		}

		point = point->next;
	}
}


bool MapPoints::init()
{	
	if (map_point_init_1(this) == false)
		return false;
	map_point_init_2(this);
	map_point_init_3(this);
	map_point_init_4(this);
	map_point_init_6(this);
	map_point_init_5(this);
	return true;
}



void MapPoints::clear()
{
	if (this->length == 0)
		return;
	if (point_list != nullptr)
		delete point_list;
	point_list = nullptr;
	point_first = nullptr;
	point_end = nullptr;
}


MapPoints::MapPoints()
{
	point_first = nullptr;
	point_list = nullptr;
	point_end = nullptr;
	length = 0;
}


MapPoints::~MapPoints()
{
	clear();
}



bool map_point_init(MapPoints *map)
{
	if (map_point_init_1(map) == false)
		return false;
	map_point_init_2(map);
	map_point_init_3(map);
	map_point_init_4(map);
	map_point_init_6(map);
	map_point_init_5(map);
	return true;
}
