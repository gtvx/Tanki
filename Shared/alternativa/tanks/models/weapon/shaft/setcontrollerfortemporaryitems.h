#ifndef SETCONTROLLERFORTEMPORARYITEMS_H
#define SETCONTROLLERFORTEMPORARYITEMS_H

#include <QHash>

class Object3D;

class SetControllerForTemporaryItems
{
	QVector<Object3D*> temporaryItems;
	QHash<Object3D*, bool> *sourceSet;
public:
	SetControllerForTemporaryItems(QHash<Object3D*, bool> *sourceSet);

	void addTemporaryItem(Object3D *object);
	void deleteAllTemporaryItems();
};

#endif // SETCONTROLLERFORTEMPORARYITEMS_H
