#include "setcontrollerfortemporaryitems.h"

SetControllerForTemporaryItems::SetControllerForTemporaryItems(QHash<Object3D*, bool> *sourceSet)
{
	this->sourceSet = sourceSet;
}

void SetControllerForTemporaryItems::addTemporaryItem(Object3D *object)
{
	this->sourceSet->insert(object, true);
	this->temporaryItems.append(object);
}

void SetControllerForTemporaryItems::deleteAllTemporaryItems()
{
	sourceSet->clear();
	temporaryItems.clear();
}
