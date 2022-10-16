#ifndef MAP_POINTS_FILE_H
#define MAP_POINTS_FILE_H

class QString;
struct MapPoint;
struct MapPoints;

bool map_points_load_file(const QString &filename, MapPoints *map);
bool map_points_write_file(const QString &filename, MapPoints *map);


#endif // MAP_POINTS_FILE_H
