#ifndef MAP_LIST_PATH_H
#define MAP_LIST_PATH_H

struct MapPoint;

struct MapListPathNode
{
	MapPoint *point;
	MapListPathNode *next;
	MapListPathNode *prev;
	MapListPathNode *nextInPool;
};

struct MapListPath
{
	MapListPathNode *first;
	MapListPathNode *end;
	unsigned int size;
	MapListPath() : first(nullptr), end(nullptr), size(0) {}
};


void map_list_path_append(MapListPath &_this, MapPoint *point);
void map_list_path_insert_first(MapListPath &_this, MapPoint *point);
void map_list_path_remove_last(MapListPath &_this);
void map_list_path_remove_first(MapListPath &_this);
void map_list_path_remove(MapListPath &_this, MapListPathNode *node);
void map_list_path_clear(MapListPath &_this);

#endif // MAP_LIST_PATH_H
