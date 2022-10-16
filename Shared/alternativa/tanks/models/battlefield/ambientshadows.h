#ifndef AMBIENTSHADOWS_H
#define AMBIENTSHADOWS_H

#include <QSet>

class Camera3D;
class Shadow;

class AmbientShadows
{
	QSet<Shadow*> shadows;
	Camera3D *camera;
	bool enabled;

public:
	AmbientShadows(Camera3D *camera);

	void add(Shadow *shadow);
	void remove(Shadow *shadow);
	void enable();
	void disable();
};

#endif // AMBIENTSHADOWS_H
