#ifndef SFXUTILS_H
#define SFXUTILS_H

class Object3D;
class Vector3;

class SFXUtils
{
public:
	static void alignObjectPlaneToView(Object3D *object, const Vector3 *objectPosition, const Vector3 *objectDirection, const Vector3 *cameraPosition);
};

#endif // SFXUTILS_H
