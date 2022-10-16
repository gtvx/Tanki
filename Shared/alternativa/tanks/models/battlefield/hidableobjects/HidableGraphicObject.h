#ifndef HIDABLEGRAPHICOBJECT_H
#define HIDABLEGRAPHICOBJECT_H

class Vector3;

class HidableGraphicObject
{
public:
	virtual void readPosition(Vector3*) = 0;
	virtual void setAlphaMultiplier(double) = 0;
};

#endif // HIDABLEGRAPHICOBJECT_H
