#ifndef PATH_NORMALIZE_H
#define PATH_NORMALIZE_H

struct MapPoint;
struct MapListPath;
struct MapListPathNode;

bool map_point_line_check(MapPoint *point_first, MapPoint *point_end);
void path_normalize(MapListPath &map_path, MapListPathNode *node_end);

#endif // PATH_NORMALIZE_H
