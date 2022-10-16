#ifndef PARTSLIST_H
#define PARTSLIST_H

#include <QString>
class GarageModel;

class PartsList
{
	//TileList *list;
	//TileList *silentList;
	//DataProvider *dp;
	//DataProvider *silentdp;
	//Array typeSort = [0, 2, 3, 4, 1, 0, 1];
	double sumDragWay;
	int previousTime;
	int currentTime;
	double scrollSpeed;
	int lastItemIndex;
	double previousPositionX;
	double currrentPositionX;
	//Object *_selectedItemID = null;
	QString ID;
	GarageModel *model;
	int _width;
	int _height;

public:
	PartsList(const QString &id);

	static const int NOT_TANK_PART = 4;
	static const int WEAPON = 1;
	static const int ARMOR = 2;
	static const int COLOR = 3;
	static const int PLUGIN = 5;
	static const int KIT = 6;


	void update(const QString &id, const QString &param, bool value);
};

#endif // PARTSLIST_H
