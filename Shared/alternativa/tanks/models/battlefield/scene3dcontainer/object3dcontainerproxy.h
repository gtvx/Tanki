#ifndef OBJECT3DCONTAINERPROXY_H
#define OBJECT3DCONTAINERPROXY_H

#include <QHash>
#include <memory>
#include "Scene3DContainer.h"

class Object3DContainer;


class Object3DContainerProxy : public Scene3DContainer
{

	QHash<Object3D*, bool> objects;
	std::shared_ptr<Object3DContainer> container;



public:

	Object3DContainerProxy(std::shared_ptr<Object3DContainer> container = nullptr);

	void addChild(Object3D*) override;
	void addChildAt(Object3D*, int) override;
	void addChildren(QVector<Object3D*> &) override;
	void removeChild(Object3D*) override;

	void setContainer(std::shared_ptr<Object3DContainer>);

	QVector<Object3D*> removeAllChildren();
};

#endif // OBJECT3DCONTAINERPROXY_H
