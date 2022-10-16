#include "hidableobject3dwrapper.h"
#include "alternativa/engine3d/core/object3d.h"


HidableObject3DWrapper::HidableObject3DWrapper(Object3D *object)
{
	this->object = object;
}

HidableObject3DWrapper::~HidableObject3DWrapper()
{

}

void HidableObject3DWrapper::readPosition(Vector3 *pos)
{
	pos->copy(&this->object->position);
}

void HidableObject3DWrapper::setAlphaMultiplier(double value)
{
	this->object->alpha = value;
}
