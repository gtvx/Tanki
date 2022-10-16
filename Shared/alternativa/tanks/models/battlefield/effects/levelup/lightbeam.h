#ifndef LIGHTBEAM_H
#define LIGHTBEAM_H

#include "alternativa/engine3d/objects/mesh.h"

class TextureMaterial;

class LightBeam : public Mesh
{
public:

	static const T *TYPE;
	const T *__type_next;


	LightBeam(const TT &t, double size);
	void init(TextureMaterial *material);
	void clear();
};

std::shared_ptr<LightBeam> new_LightBeam(double size);

#endif // LIGHTBEAM_H
