#ifndef SCENE3DCONTAINER_H
#define SCENE3DCONTAINER_H

#include <QVector>

class Object3D;

class Scene3DContainer
{
public:
	virtual void addChild(Object3D*) = 0;
	virtual void addChildAt(Object3D*, int) = 0;
	virtual void addChildren(QVector<Object3D*>&) = 0;
	virtual void removeChild(Object3D*) = 0;
};

#endif // SCENE3DCONTAINER_H
