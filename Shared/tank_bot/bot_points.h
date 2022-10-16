#ifndef BOT_POINTS_H
#define BOT_POINTS_H

class BattlefieldModel;
class MapPoints;

void bot_points_save(class BattlefieldModel *battlefieldModel, MapPoints *map);
void bot_points_load(class BattlefieldModel *battlefieldModel, MapPoints *map);

#endif // BOT_POINTS_H
