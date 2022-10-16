#ifndef PARTICLE_H
#define PARTICLE_H


#include "alternativa/tanks/engine3d/animatedsprite3d.h"
class ColorTransformEntry;

class Particle : public AnimatedSprite3D
{
public:

	static const T *TYPE;
	const T *__type_next;


	Particle(const TT &t, TextureAnimation *animData);

	Vector3 velocity;
	double distance;
	double currFrame;

	void dispose();
	void updateColorTransofrm(double maxDistance, QVector<ColorTransformEntry*> &points);

private:
	void interpolateColorTransform(const ColorTransformEntry *ct1, const ColorTransformEntry *ct2, double t, ColorTransform *result);
	void copyStructToColorTransform(const ColorTransformEntry *source, ColorTransform *result);
};

std::shared_ptr<Particle> new_Particle(TextureAnimation *animData);

#endif // PARTICLE_H
