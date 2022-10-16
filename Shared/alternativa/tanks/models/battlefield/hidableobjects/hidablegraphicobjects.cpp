#include "hidablegraphicobjects.h"
#include "HidableGraphicObject.h"
#include "mymath.h"


HidableGraphicObjects::HidableGraphicObjects()
{
	this->enabled = false;
	this->numObjects = 0;
}

void HidableGraphicObjects::add(HidableGraphicObject *object)
{
	if (this->objects.indexOf(object) < 0)
	{
		this->objects.append(object);
		this->numObjects++;
		//var _local_2:* = this->numObjects++;
		//this->objects[_local_2] = object;
	}
}

void HidableGraphicObjects::remove(HidableGraphicObject *object)
{
	int i = this->objects.indexOf(object);
	if (i >= 0)
	{
		object->setAlphaMultiplier(1);
		this->numObjects--;
		this->objects[i] = this->objects[this->numObjects];
		//this->objects[this->numObjects] = nullptr;
		this->objects.removeLast();
	}
}

void HidableGraphicObjects::setCenterAndRadius(const Vector3 *center, double value)
{
	this->center.copy(center);
	this->hideRadiusSquared = value * value;
}

void HidableGraphicObjects::restore()
{
	for (int i = 0; i < this->numObjects; i++)
	{
		HidableGraphicObject *object = this->objects[i];
		object->setAlphaMultiplier(1);
	}
}

void HidableGraphicObjects::render(int time, int deltaMsec)
{
	(void)time;
	(void)deltaMsec;

	for (int i = 0; i < this->numObjects; i++)
	{
		HidableGraphicObject *object = this->objects[i];
		Vector3 objectPosition;
		object->readPosition(&objectPosition);
		object->setAlphaMultiplier(this->getAlphaMultiplier(&objectPosition));
	}
}

double HidableGraphicObjects::getAlphaMultiplier(const Vector3 *v)
{
	double _loc2_ = v->x - this->center.x;
	double _loc3_ = v->y - this->center.y;
	double _loc4_ = v->z - this->center.z;
	double _loc5_ = (((_loc2_ * _loc2_) + (_loc3_ * _loc3_)) + (_loc4_ * _loc4_));
	if (_loc5_ < this->hideRadiusSquared)
	{
		return MyMath::sqrt((_loc5_ / this->hideRadiusSquared));
	}
	return 1;
}

void HidableGraphicObjects::clear()
{
	this->objects.clear();
	this->numObjects = 0;
}
