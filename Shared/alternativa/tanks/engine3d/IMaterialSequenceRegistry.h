#ifndef IMATERIALSEQUENCEREGISTRY_H
#define IMATERIALSEQUENCEREGISTRY_H

class QString;
enum class MaterialType;
class BitmapData;
class MaterialSequence;

class IMaterialSequenceRegistry
{
public:
	virtual void setTimerInterval(int) = 0;
	virtual bool getUseMipMapping() = 0;
	virtual void setUseMipMapping(bool) = 0;
	virtual MaterialSequence* getSequence(MaterialType, BitmapData*, int, double, bool = false, bool = false) = 0;
	virtual MaterialSequence* getSquareSequence(MaterialType, BitmapData*, double, bool = false, bool = false) = 0;
	virtual void clear() = 0;
	virtual QString getDump() = 0;
	virtual void disposeSequence(MaterialSequence*) = 0;
};


#endif // IMATERIALSEQUENCEREGISTRY_H
