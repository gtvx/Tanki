#ifndef ISOUND3DEFFECT_H
#define ISOUND3DEFFECT_H

#include "ISpecialEffect.h"

class Vector3;

class ISound3DEffect : public ISpecialEffect
{
public:
	virtual void setEnabled(bool) = 0;
	virtual void readPosition(Vector3*) = 0;
	virtual int getNumSounds() = 0;
};

#endif // ISOUND3DEFFECT_H
