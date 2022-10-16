#ifndef ASTAR_N5_H
#define ASTAR_N5_H

#include <vector>
#include "map_points.h"

struct MapListPath;
class AStarListMapPoint;

class AStar
{
	AStarListMapPoint *open_list;
public:

	AStar();
	~AStar();

	bool find(MapPoint *pointStart,
			  MapPoint *pointEnd,
			  uint32_t maxNumberExamineCells,
			  MapListPath &path);
};

#endif // ASTAR_N5_H
