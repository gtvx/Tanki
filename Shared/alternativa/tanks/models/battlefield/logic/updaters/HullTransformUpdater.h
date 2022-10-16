#ifndef HULLTRANSFORMUPDATER_H
#define HULLTRANSFORMUPDATER_H

class HullTransformUpdater
{
public:
	virtual void reset() = 0;
	virtual void update(double) = 0;
	virtual ~HullTransformUpdater() {}
};

#endif // HULLTRANSFORMUPDATER_H
