#ifndef ICAMERASTATEMODIFIER_H
#define ICAMERASTATEMODIFIER_H

class Vector3;
class IFollowCameraController;

class ICameraStateModifier
{
public:
	virtual bool update(int, int, Vector3*, Vector3*) = 0;
	virtual void onAddedToController(IFollowCameraController*) = 0;
	virtual void destroy() = 0;
};

#endif // ICAMERASTATEMODIFIER_H
