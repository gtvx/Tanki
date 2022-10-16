#include "objects2tank.h"

Objects2Tank::Objects2Tank()
{

}

void Objects2Tank::add(Object3D *object, Tank *tank)
{
	list.insert(object, tank);
}

Tank* Objects2Tank::get(Object3D *object)
{
	return list.value(object);
}
