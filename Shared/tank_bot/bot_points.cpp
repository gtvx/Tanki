#include "bot_points.h"
#include "map_points_file.h"
#include "get_dir.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include <QString>

#define dir_map_points (get_dir() + "/map_points/")

static QString GetFileNameMapPointsBin(class BattlefieldModel *battlefieldModel)
{
	QString map_id = battlefieldModel->getMapResourceId();
	return map_id + "_points.bin";
}

void bot_points_save(class BattlefieldModel *battlefieldModel, MapPoints *map)
{
	QString file = GetFileNameMapPointsBin(battlefieldModel);
	if (file.isEmpty())
		return;
	map_points_write_file(dir_map_points + file, map);
}

void bot_points_load(class BattlefieldModel *battlefieldModel, MapPoints *map)
{
	QString file = GetFileNameMapPointsBin(battlefieldModel);
	if (file.isEmpty())
		return;
	map_points_load_file(dir_map_points + file, map);
}
