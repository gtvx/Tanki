#ifndef ANIMATEDPLANE_H
#define ANIMATEDPLANE_H

#include "alternativa/tanks/sfx/simpleplane.h"
#include <QVector>


class AnimatedPlane : public SimplePlane
{
	QVector<std::shared_ptr<Material>> *materials;
	int numFrames;
public:

	static const T *TYPE;
	const T *__type_next;


	double currFrame;
	AnimatedPlane(const TT &t, double width, double length);
	void setMaterials(QVector<std::shared_ptr<Material>> *materials);
	void clearMaterials();
	void update(double dt, double fps);
};

std::shared_ptr<AnimatedPlane> new_AnimatedPlane(double width, double length);

#endif // ANIMATEDPLANE_H
