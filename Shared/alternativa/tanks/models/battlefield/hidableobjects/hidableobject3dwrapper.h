#ifndef HIDABLEOBJECT3DWRAPPER_H
#define HIDABLEOBJECT3DWRAPPER_H

#include "HidableGraphicObject.h"

class Object3D;
class Vector3;

class HidableObject3DWrapper : public HidableGraphicObject
{
	Object3D *object;
public:
	HidableObject3DWrapper(Object3D *object);
	virtual ~HidableObject3DWrapper();
	void readPosition(Vector3 *pos) override;
	void setAlphaMultiplier(double value) override;
};

#endif // HIDABLEOBJECT3DWRAPPER_H
