#include "path_normalize.h"
#include "map_points.h"
#include "map_list_path.h"
#include <math.h>

extern MapListPath map_path;


static inline bool line_check(MapPoint *point_a, MapPoint *point_b, MapPoint *point_c)
{
	return ((point_b->x - point_a->x)*(point_c->y - point_a->y) - (point_b->y - point_a->y)*(point_c->x - point_a->x)) > 0;
}


static inline bool check_point_bad(MapPoint *point)
{
	return point->locked;
}



bool map_point_line_check(MapPoint *point_first, MapPoint *point_end)
{
	int32_t x = point_end->x - point_first->x;
	int32_t y = point_end->y - point_first->y;

	uint32_t l = abs(point_end->x - point_first->x) + abs(point_end->y - point_first->y);
	if (l < 3) {
		if (abs(x) == 1 && abs(y) == 1) {
			return true;
		}
		if ((abs(x) + abs(y)) == 1)
			return true;
	}

	MapPoint *point_center = nullptr;
	MapPoint *point1 = nullptr;
	MapPoint *point2 = nullptr;
	bool ok_0 = false;
	bool ok_1 = false;
	bool ok_2 = false;

	if (y < 0)
	{
		if (x < 0)
		{
			if (y < x)
			{
				point_center = point_first->XM_YM;
				while (point_center != nullptr)
				{
					MapPoint *point_a = point_center->YM;
					MapPoint *point_b = point_center->XM_YM;
					if (point_a == point_end || point_b == point_end) {
						ok_1 = true;
						break;
					}
					if (point_a != nullptr && line_check(point_first, point_end, point_a) == false) {
						if (check_point_bad(point_a))
							break;
						point_center = point_a;
					} else {
						if (point_b != nullptr && line_check(point_first, point_end, point_b) == false) {
							if (check_point_bad(point_b))
								break;
							point_center = point_b;
						} else
							break;
					}
				}

				point_center = point_first->YM;
				while (point_center != nullptr)
				{
					MapPoint *point_a = point_center->XM_YM;
					MapPoint *point_b = point_center->YM;
					if (point_a == point_end || point_b == point_end) {
						ok_2 = true;
						break;
					}

					if (point_a != nullptr && line_check(point_first, point_end, point_a) == true) {
						if (check_point_bad(point_a))
							break;
						point_center = point_a;
					} else {
						if (point_b != nullptr && line_check(point_first, point_end, point_b) == true) {
							if (check_point_bad(point_b))
								break;
							point_center = point_b;
						} else
							break;
					}
				}

			} else if (y > x)
			{
				point1 = point_first->XM;
				point2 = point_first->XM_YM;

				point_center = point1;
				while (point_center != nullptr)
				{
					MapPoint *point_a = point_center->XM_YM;
					MapPoint *point_b = point_center->XM;
					if (point_a == point_end || point_b == point_end) {
						ok_1 = true;
						break;
					}
					if (point_a != nullptr && line_check(point_first, point_end, point_a) == false) {
						if (check_point_bad(point_a))
							break;
						point_center = point_a;
					} else {
						if (point_b != nullptr && line_check(point_first, point_end, point_b) == false) {
							if (check_point_bad(point_b))
								break;
							point_center = point_b;
						} else
							break;
					}
				}


				point_center = point2;
				while (point_center != nullptr)
				{
					MapPoint *point_a = point_center->XM;
					MapPoint *point_b = point_center->XM_YM;
					if (point_a == point_end || point_b == point_end) {
						ok_2 = true;
						break;
					}
					if (point_a != nullptr && line_check(point_first, point_end, point_a) == true) {
						if (check_point_bad(point_a))
							break;
						point_center = point_a;
					} else {
						if (point_b != nullptr && line_check(point_first, point_end, point_b) == true) {
							if (check_point_bad(point_b))
								break;
							point_center = point_b;
						} else
							break;
					}
				}


			} else if (y == x)
			{
				point1 = point_first->XM_YM;

				point_center = point1;
				while (point_center != nullptr)
				{
					MapPoint *point = point_center->XM_YM;
					if (point == nullptr)
						break;
					if (check_point_bad(point))
						break;
					if (point == point_end) {
						ok_0 = true;
						break;
					}
					point_center = point;
				}
			}
		} else if (x == 0)
		{
			point1 = point_first->YM;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point = point_center->YM;
				if (point == nullptr)
					break;
				if (check_point_bad(point))
					break;
				if (point == point_end) {
					ok_0 = true;
					break;
				}
				point_center = point;
			}
		} else if (x > 0)
		{
			if (y == -x)
			{
				point1 = point_first->XP_YM;

				point_center = point1;
				while (point_center != nullptr)
				{
					MapPoint *point = point_center->XP_YM;
					if (point == nullptr)
						break;
					if (check_point_bad(point))
						break;
					if (point == point_end) {
						ok_0 = true;
						break;
					}
					point_center = point;
				}
			} else if (y < -x)
			{
				point1 = point_first->YM;
				point2 = point_first->XP_YM;

				point_center = point1;
				while (point_center != nullptr)
				{
					MapPoint *point_a = point_center->XP_YM;
					MapPoint *point_b = point_center->YM;
					if (point_a == point_end || point_b == point_end) {
						ok_1 = true;
						break;
					}
					if (point_a != nullptr && line_check(point_first, point_end, point_a) == false) {
						if (check_point_bad(point_a))
							break;
						point_center = point_a;
					} else {
						if (point_b != nullptr && line_check(point_first, point_end, point_b) == false) {
							if (check_point_bad(point_b))
								break;
							point_center = point_b;
						} else
							break;
					}
				}

				point_center = point2;
				while (point_center != nullptr)
				{
					MapPoint *point_a = point_center->YM;
					MapPoint *point_b = point_center->XP_YM;
					if (point_a == point_end || point_b == point_end) {
						ok_2 = true;
						break;
					}
					if (point_a != nullptr && line_check(point_first, point_end, point_a) == true) {
						if (check_point_bad(point_a))
							break;
						point_center = point_a;
					} else {
						if (point_b != nullptr && line_check(point_first, point_end, point_b) == true) {
							if (check_point_bad(point_b))
								break;
							point_center = point_b;
						} else
							break;
					}
				}

			} else if (y < x)
			{
				point1 = point_first->XP_YM;
				point2 = point_first->XP;

				point_center = point1;
				while (point_center != nullptr)
				{
					MapPoint *point_a = point_center->XP;
					MapPoint *point_b = point_center->XP_YM;
					if (point_a == point_end || point_b == point_end) {
						ok_1 = true;
						break;
					}
					if (point_a != nullptr && line_check(point_first, point_end, point_a) == false) {
						if (check_point_bad(point_a))
							break;
						point_center = point_a;
					} else {

						if (point_b != nullptr && line_check(point_first, point_end, point_b) == false) {
							if (check_point_bad(point_b))
								break;
							point_center = point_b;
						} else
							break;
					}
				}

				point_center = point2;
				while (point_center != nullptr)
				{
					MapPoint *point_a = point_center->XP_YM;
					MapPoint *point_b = point_center->XP;
					if (point_a == point_end || point_b == point_end) {
						ok_2 = true;
						break;
					}
					if (point_a != nullptr && line_check(point_first, point_end, point_a) == true) {
						if (check_point_bad(point_a))
							break;
						point_center = point_a;
					} else {
						if (point_b != nullptr && line_check(point_first, point_end, point_b) == true) {
							if (check_point_bad(point_b))
								break;
							point_center = point_b;
						} else
							break;
					}
				}
			}
		}
	} else if (y == 0)
	{
		if (x > 0)
		{
			point1 = point_first->XP;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point = point_center->XP;
				if (point == nullptr)
					break;
				if (check_point_bad(point))
					break;
				if (point == point_end) {
					ok_0 = true;
					break;
				}
				point_center = point;
			}
		} else if (x < 0)
		{
			point1 = point_first->XM;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point = point_center->XM;
				if (point == nullptr)
					break;
				if (check_point_bad(point))
					break;
				if (point == point_end) {
					ok_0 = true;
					break;
				}
				point_center = point;
			}
		}
	} else if (y > 0)
	{
		if (y == x)
		{
			point1 = point_first->XP_YP;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point = point_center->XP_YP;
				if (point == nullptr)
					break;
				if (check_point_bad(point))
					break;
				if (point == point_end) {
					ok_0 = true;
					break;
				}
				point_center = point;
			}
		} else if (y < x)
		{
			point1 = point_first->XP;
			point2 = point_first->XP_YP;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point_a = point_center->XP_YP;
				MapPoint *point_b = point_center->XP;
				if (point_a == point_end || point_b == point_end) {
					ok_1 = true;
					break;
				}
				if (point_a != nullptr && line_check(point_first, point_end, point_a) == false) {
					if (check_point_bad(point_a))
						break;
					point_center = point_a;
				} else {
					if (point_b != nullptr && line_check(point_first, point_end, point_b) == false) {
						if (check_point_bad(point_b))
							break;
						point_center = point_b;
					} else
						break;
				}
			}

			point_center = point2;
			while (point_center != nullptr)
			{
				MapPoint *point_a = point_center->XP;
				MapPoint *point_b = point_center->XP_YP;
				if (point_a == point_end || point_b == point_end) {
					ok_2 = true;
					break;
				}
				if (point_a != nullptr && line_check(point_first, point_end, point_a) == true) {
					if (check_point_bad(point_a))
						break;
					point_center = point_a;
				} else {
					if (point_b != nullptr && line_check(point_first, point_end, point_b) == true) {
						if (check_point_bad(point_b))
							break;
						point_center = point_b;
					} else
						break;
				}
			}



		} else if (x > 0) {
			point1 = point_first->XP_YP;
			point2 = point_first->YP;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point_a = point_center->YP;
				MapPoint *point_b = point_center->XP_YP;
				if (point_a == point_end || point_b == point_end) {
					ok_1 = true;
					break;
				}
				if (point_a != nullptr && line_check(point_first, point_end, point_a) == false) {
					if (check_point_bad(point_a))
						break;
					point_center = point_a;
				} else {
					if (point_b != nullptr && line_check(point_first, point_end, point_b) == false) {
						if (check_point_bad(point_b))
							break;
						point_center = point_b;
					} else
						break;
				}
			}


			point_center = point2;
			while (point_center != nullptr)
			{
				MapPoint *point_a = point_center->XP_YP;
				MapPoint *point_b = point_center->YP;
				if (point_a == point_end || point_b == point_end) {
					ok_2 = true;
					break;
				}
				if (point_a != nullptr && line_check(point_first, point_end, point_a) == true) {
					if (check_point_bad(point_a))
						break;
					point_center = point_a;
				} else {
					if (point_b != nullptr && line_check(point_first, point_end, point_b) == true) {
						if (check_point_bad(point_b))
							break;
						point_center = point_b;
					} else
						break;
				}
			}



		}  else if (x == 0) {
			point1 = point_first->YP;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point = point_center->YP;
				if (point == nullptr)
					break;
				if (check_point_bad(point))
					break;
				if (point == point_end) {
					ok_0 = true;
					break;
				}
				point_center = point;
			}
		} else if (y > -x) {
			point1 = point_first->YP;
			point2 = point_first->XM_YP;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point_a = point_center->XM_YP;
				MapPoint *point_b = point_center->YP;
				if (point_a == point_end || point_b == point_end) {
					ok_1 = true;
					break;
				}
				if (point_a != nullptr && line_check(point_first, point_end, point_a) == false) {
					if (check_point_bad(point_a))
						break;
					point_center = point_a;
				} else {
					if (point_b != nullptr && line_check(point_first, point_end, point_b) == false) {
						if (check_point_bad(point_b))
							break;
						point_center = point_b;
					} else
						break;
				}
			}

			point_center = point2;
			while (point_center != nullptr)
			{
				MapPoint *point_a = point_center->YP;
				MapPoint *point_b = point_center->XM_YP;
				if (point_a == point_end || point_b == point_end) {
					ok_2 = true;
					break;
				}
				if (point_a != nullptr && line_check(point_first, point_end, point_a) == true) {
					if (check_point_bad(point_a))
						break;
					point_center = point_a;
				} else {
					if (point_b != nullptr && line_check(point_first, point_end, point_b) == true) {
						if (check_point_bad(point_b))
							break;
						point_center = point_b;
					} else
						break;
				}
			}

		} else if (y == -x) {
			point1 = point_first->XM_YP;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point = point_center->XM_YP;
				if (point == nullptr)
					break;
				if (check_point_bad(point))
					break;
				if (point == point_end) {
					ok_0 = true;
					break;
				}
				point_center = point;
			}
		} else if (x < 0) {
			point1 = point_first->XM_YP;
			point2 = point_first->XM;

			point_center = point1;
			while (point_center != nullptr)
			{
				MapPoint *point_a = point_center->XM;
				MapPoint *point_b = point_center->XM_YP;
				if (point_a == point_end || point_b == point_end) {
					ok_1 = true;
					break;
				}
				if (point_a != nullptr && line_check(point_first, point_end, point_a) == false) {
					if (check_point_bad(point_a))
						break;
					point_center = point_a;
				} else {
					if (point_b != nullptr && line_check(point_first, point_end, point_b) == false) {
						if (check_point_bad(point_b))
							break;
						point_center = point_b;
					} else
						break;
				}
			}

			point_center = point2;
			while (point_center != nullptr)
			{
				MapPoint *point_a = point_center->XM_YP;
				MapPoint *point_b = point_center->XM;
				if (point_a == point_end || point_b == point_end) {
					ok_2 = true;
					break;
				}
				if (point_a != nullptr && line_check(point_first, point_end, point_a) == true) {
					if (check_point_bad(point_a))
						break;
					point_center = point_a;
				} else {
					if (point_b != nullptr && line_check(point_first, point_end, point_b) == true) {
						if (check_point_bad(point_b))
							break;
						point_center = point_b;
					} else
						break;
				}
			}

		}
	}

	return ok_0 || (ok_1 && ok_2);
}




void path_normalize(MapListPath &map_path, MapListPathNode *node_end)
{
	bool change;
	do
	{
		change = false;
		//MapListPathNode *node_1 = map_path.first;
		MapListPathNode *node_1 = node_end;

		while (node_1 != nullptr)
		{
			//MapListPathNode *node_2 = node_1->next;
			MapListPathNode *node_2 = node_1->prev;
			if (node_2 == nullptr)
				break;

			//MapListPathNode *node_3 = node_2->next;
			MapListPathNode *node_3 = node_2->prev;
			if (node_3 == nullptr)
				break;

			MapPoint *point_1 = node_1->point;
			MapPoint *point_2 = node_2->point;
			MapPoint *point_3 = node_3->point;

			int32_t z = point_1->z;

			if (point_2->z == z && point_3->z == z)
			{
				if (map_point_line_check(point_1, point_3))
				{
					map_list_path_remove(map_path, node_2);
					change = true;
				}
			}

			//node_1 = node_1->next;
			node_1 = node_1->prev;
		}
	} while (change == true);
}
