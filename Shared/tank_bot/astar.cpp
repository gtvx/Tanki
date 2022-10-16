#include "astar.h"
#include "map_list_path.h"
#include <string.h>

static int32_t my_abs(int32_t v)
{
	return v >= 0 ? v : -v;
}

static MapPoint* GetPoint(MapPoint *p)
{
	if (p == nullptr)
		return nullptr;
	if (p->locked == true)
		return nullptr;
	return p;
}

static MapPoint* Get(MapPoint *current, uint8_t p, bool &diagonal)
{
	switch (p)
	{
		case 0:
			diagonal = false;
			return GetPoint(current->XP);
		case 1:
			diagonal = false;
			return GetPoint(current->YP);
		case 2:
			diagonal = false;
			return GetPoint(current->XM);
		case 3:
			diagonal = false;
			return GetPoint(current->YM);
		case 4:
			diagonal = true;
			return GetPoint(current->XP_YP);
		case 5:
			diagonal = true;
			return GetPoint(current->XP_YM);
		case 6:
			diagonal = true;
			return GetPoint(current->XM_YP);
		case 7:
			diagonal = true;
			return GetPoint(current->XM_YM);
	}

	return nullptr;
}



static void add_free(MapPoint *&end_clear, MapPoint *point)
{
	if (point->astar_add_free == false)
	{
		point->astar_add_free = true;
		if (end_clear->astar_next_free != nullptr)
		{
			//qDebug("error add free");
		}

		end_clear->astar_next_free = point;
		end_clear = point;
	}
}


class AStarListMapPoint
{
	uint32_t _alloc;
	uint32_t _count;
	MapPoint **_list;
public:

	AStarListMapPoint()
	{
		_alloc = 10;
		_count = 0;
		_list = new MapPoint*[_alloc];
	}

	~AStarListMapPoint()
	{
		delete []_list;
	}

	void append(MapPoint *item)
	{
		if (_count >= _alloc)
		{
			_alloc *= 2;
			MapPoint **temp = new MapPoint*[_alloc];
			memcpy(temp, _list, sizeof(MapPoint*) * _count);
			delete _list;
			_list = temp;
		}

		_list[_count++] = item;
	}

	MapPoint* last()
	{
		return _list[_count-1];
	}

	void removeLast()
	{
		_count--;
	}

	void replace(int index, MapPoint *item)
	{
		_list[index] = item;
	}

	MapPoint* at(int index)
	{
		return _list[index];
	}

	int count()
	{
		return _count;
	}

	bool isEmpty()
	{
		return _count == 0;
	}

	void clear()
	{
		_count = 0;
	}

	void swap(uint32_t index_a, uint32_t index_b)
	{
		MapPoint *temp = _list[index_a];
		_list[index_a] = _list[index_b];
		_list[index_b] = temp;
	}
};


AStar::AStar()
{
	open_list = new AStarListMapPoint;
}

AStar::~AStar()
{
	delete open_list;
}


bool AStar::find(MapPoint *point_start,
				MapPoint *point_end,
				uint32_t max_number_examine_cells,
				MapListPath &path)
{
	map_list_path_clear(path);

	open_list->clear();

	uint32_t number_of_close_list_items = 0;
	bool path_found = false;
	bool overcharge_number_examine_cells = false;
	MapPoint *start_point = point_start;
	MapPoint *first_clear = start_point;
	MapPoint *end_clear = start_point;
	first_clear->astar_add_free = true;
	start_point->astar_Gcost = 0;
	open_list->append(start_point);

	int32_t start_x = point_start->_x;
	int32_t start_y = point_start->_y;
	int32_t start_z = point_start->_z;
	int32_t target_x = point_end->_x;
	int32_t target_y = point_end->_y;
	int32_t target_z = point_end->_z;

	uint32_t minimumH = 10 * (my_abs(start_x - target_x) + my_abs(start_y - target_y) + my_abs(start_z - target_z));

	MapPoint *minimum_H_point = start_point;
	MapPoint *point_list_end = start_point;

	for (;;)
	{
		if (open_list->isEmpty() == true)
		{
			overcharge_number_examine_cells = true;
			path_found = true;
			break;
		}


		if (number_of_close_list_items > max_number_examine_cells)
		{
			overcharge_number_examine_cells = true;
			path_found = true;
			break;
		}



		MapPoint *parent_point = open_list->at(0);
		add_free(end_clear, parent_point);

		parent_point->astar_status = MapPoint::CLOSED;

		open_list->replace(0, open_list->last());
		number_of_close_list_items++;
		open_list->removeLast();

		const uint32_t open_list_count = open_list->count();

		for (uint32_t v = 0;;)
		{
			const uint32_t u = v;
			const uint32_t u_2 = u << 1; //u * 2
			const uint32_t u_2_1 = u_2 + 1;

			if (u_2_1 < open_list_count) {
				if (open_list->at(u)->astar_Fcost >= open_list->at(u_2)->astar_Fcost)
					v = u_2;

				if (open_list->at(v)->astar_Fcost >= open_list->at(u_2_1)->astar_Fcost)
					v = u_2_1;

			} else if (u_2 < open_list_count)
				if (open_list->at(u)->astar_Fcost >= open_list->at(u_2)->astar_Fcost)
					v = u_2;

			if (u == v)
				break;


			open_list->swap(u, v);
		}


		for (uint8_t i = 0; i < 8; i++)
		{
			bool diagonal;

			MapPoint *current_point = Get(parent_point, i, diagonal);

			if (current_point == nullptr)
				continue;

			const MapPoint::Status status = current_point->astar_status;

			if (status == MapPoint::CLOSED)
				continue;

			if (status == MapPoint::EMPTY)
			{
				add_free(end_clear, current_point);

				point_list_end->astar_next = current_point;
				point_list_end = current_point;

				open_list->append(current_point);

				MapPoint *open_id_last = current_point;

				uint8_t added_G_cost = diagonal == true ? 14 : 10;

				current_point->astar_Gcost = parent_point->astar_Gcost + added_G_cost;

				const uint32_t temp_h = 10 * (my_abs(current_point->_x - target_x) + my_abs(current_point->_y - target_y) + my_abs(current_point->_z - target_z));


				if (temp_h < minimumH)
				{
					minimumH = temp_h;
					minimum_H_point = current_point;
				}

				open_id_last->astar_Hcost = temp_h;
				open_id_last->astar_Fcost = current_point->astar_Gcost + open_id_last->astar_Hcost;

				current_point->astar_parent = parent_point;

				uint32_t m = open_list->count() - 1;
				while (m != 0)
				{
					uint32_t parent_index = m >> 1; // m / 2
					if (open_list->at(m)->astar_Fcost > open_list->at(parent_index)->astar_Fcost)
						break;

					open_list->swap(parent_index, m);

					m = parent_index;
				}

				current_point->astar_status = MapPoint::OPEN;
			}
			else
			{
				//OPEN

				uint8_t added_g_cost = diagonal == true ? 14 : 10;

				const uint32_t temp_g_cost = parent_point->astar_Gcost + added_g_cost;

				if (temp_g_cost < current_point->astar_Gcost)
				{
					current_point->astar_parent = parent_point;
					current_point->astar_Gcost = temp_g_cost;

					const uint32_t open_listCount = open_list->count();

					for (uint32_t i = 0; i < open_listCount; i++)
					{
						MapPoint *current_open = open_list->at(i);

						if (current_open->_x != current_point->_x || current_open->_y != current_point->_y || current_open->_z != current_point->_z)
							continue;

						current_open->astar_Fcost = current_point->astar_Gcost + current_open->astar_Hcost;

						uint32_t m = i;
						uint32_t parent_index = m >> 1; // m / 2

						while (m != 0)
						{
							if (open_list->at(m)->astar_Fcost < open_list->at(parent_index)->astar_Fcost)
							{
								open_list->swap(parent_index, m);
								m = parent_index;
								parent_index = m >> 1; // m / 2
							}
							else
								break;
						}

						break;
					}
				}
			}
		}


		if (point_end->astar_status == MapPoint::OPEN)
		{
			path_found = true;
			break;
		}
	}

	if (path_found == true)
	{
		if (overcharge_number_examine_cells == true)
			point_end = minimum_H_point;

		MapPoint *current_point = point_end;

		while (current_point != nullptr)
		{
			map_list_path_insert_first(path, current_point);
			current_point = current_point->astar_parent;
		}
	}


	{
		MapPoint *current_point = first_clear;
		while (current_point != nullptr)
		{
			MapPoint *next = current_point->astar_next_free;
			current_point->astar_status = MapPoint::EMPTY;
			current_point->astar_Gcost = 0;
			current_point->astar_Fcost = 0;
			current_point->astar_Hcost = 0;
			current_point->astar_parent = nullptr;
			current_point->astar_next = nullptr;
			current_point->astar_next_free = nullptr;
			current_point->astar_add_free = false;
			current_point = next;
		}
	}

	return overcharge_number_examine_cells == false;
}

