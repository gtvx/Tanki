#ifndef MAP_POINTS_H
#define MAP_POINTS_H


#include <stdint.h>
class TanksCollisionDetector;


class MapPoint
{
public:

	MapPoint *XP;
	MapPoint *XM;
	MapPoint *YP;
	MapPoint *YM;

	MapPoint *XP_YP;
	MapPoint *XP_YM;
	MapPoint *XM_YP;
	MapPoint *XM_YM;

	MapPoint *next;
	MapPoint *prev;



	MapPoint *astar_next_free;

	int32_t x;
	int32_t y;
	int32_t z;

	bool block_xp;
	bool block_xm;
	bool block_yp;
	bool block_ym;

//------------------------
	enum Status : uint8_t
	{
		CLOSED,
		OPEN,
		EMPTY,
	};

	MapPoint *astar_parent;
	MapPoint *astar_next;
	uint32_t astar_Gcost;
	uint32_t astar_Fcost;
	uint32_t astar_Hcost;
	Status astar_status;
	bool astar_add_free;

//------------------------

	int32_t _x;
	int32_t _y;
	int32_t _z;

	uint32_t INDEX;
	uint32_t INDEX_XP;
	uint32_t INDEX_XM;
	uint32_t INDEX_YP;
	uint32_t INDEX_YM;

	bool locked;
	uint8_t diagonal;

	MapPoint();

	void reset();
};

class MapPoints
{
public:
	MapPoint *point_first;
	MapPoint *point_list;
	MapPoint *point_end;
	uint32_t length;
	TanksCollisionDetector *collisionDetector;
	MapPoints();
	~MapPoints();
	void create(double x, double y, double z);
	bool init();
	void clear();
};

#endif // MAP_POINTS_H
