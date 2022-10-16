#ifndef OBJECTS2TANK_H
#define OBJECTS2TANK_H

#include <QHash>

class Object3D;
class Tank;

class Objects2Tank
{
	QHash<Object3D*, Tank*> list;
public:
	Objects2Tank();

	void add(Object3D*, Tank*);

	Tank* get(Object3D*);
};

#endif // OBJECTS2TANK_H
