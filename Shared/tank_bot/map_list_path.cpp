#include "map_list_path.h"
#include "map_points.h"

static MapListPathNode *poolTop = nullptr;

static MapListPathNode* create()
{
	if (poolTop == nullptr)
		return new MapListPathNode;

	MapListPathNode *node = poolTop;
	::poolTop = ::poolTop->nextInPool;
	return node;
}

static void dispose(MapListPathNode *_this)
{
	_this->nextInPool = ::poolTop;
	::poolTop = _this;
}

void map_list_path_append(MapListPath &_this, MapPoint *point)
{
	MapListPathNode *node = create();
	node->next = nullptr;
	node->point = point;

	MapListPathNode *end = _this.end;
	node->prev = end;

	if (end != nullptr)
		end->next = node;

	if (_this.first == nullptr)
		_this.first = node;

	_this.size++;
	_this.end = node;
}

void map_list_path_insert_first(MapListPath &_this, MapPoint *point)
{
	MapListPathNode *node = create();
	node->point = point;
	node->prev = nullptr;

	MapListPathNode *first = _this.first;
	node->next = first;

	if (first == nullptr)
		_this.end = node;
	else
		first->prev = node;

	_this.size++;
	_this.first = node;
}

void map_list_path_remove_last(MapListPath &_this)
{
	MapListPathNode *end = _this.end;
	if (end == nullptr)
		return;

	MapListPathNode *prev = end->prev;
	_this.end = prev;

	if (prev == nullptr)
		_this.first = nullptr;
	else
		prev->next = nullptr;

	_this.size--;
	dispose(end);
}

void map_list_path_remove_first(MapListPath &_this)
{
	MapListPathNode *first = _this.first;
	if (first == nullptr)
		return;

	MapListPathNode *next = first->next;
	_this.first = next;

	if (next == nullptr)
		_this.end = nullptr;
	else
		next->prev = nullptr;

	_this.size--;
	dispose(first);
}

void map_list_path_remove(MapListPath &_this, MapListPathNode *node)
{
	if (node->prev != nullptr)
		node->prev->next = node->next;
	if (node->next != nullptr)
		node->next->prev = node->prev;

	if (_this.first == node)
		_this.first = node->next;

	if (_this.end == node)
		_this.end = node->prev;

	dispose(node);
	_this.size--;
}

void map_list_path_clear(MapListPath &_this)
{
	if (_this.size == 0)
		return;

	MapListPathNode *node = _this.first;

	while (node != nullptr)
	{
		dispose(node);
		node = node->next;
	}

	_this.first = nullptr;
	_this.end = nullptr;
	_this.size = 0;
}
