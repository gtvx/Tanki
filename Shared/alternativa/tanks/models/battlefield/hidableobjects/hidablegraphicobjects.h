#ifndef HIDABLEGRAPHICOBJECTS_H
#define HIDABLEGRAPHICOBJECTS_H

#include "alternativa/math/vector3.h"
#include <QVector>

class HidableGraphicObject;

class HidableGraphicObjects
{
	Vector3 center;
	QVector<HidableGraphicObject*> objects;
	double hideRadiusSquared;
	int numObjects;
	bool enabled;
public:
	HidableGraphicObjects();

	void add(HidableGraphicObject *object);
	void remove(HidableGraphicObject *object);
	void setCenterAndRadius(const Vector3 *center, double value);
	void restore();
	void render(int time, int deltaMsec);
	double getAlphaMultiplier(const Vector3 *param1);
	void clear();
	void enable() { this->enabled = true; }
	void disnable() { this->enabled = false; }
	bool isEnabled() { return this->enabled; }
};

#endif // HIDABLEGRAPHICOBJECTS_H
