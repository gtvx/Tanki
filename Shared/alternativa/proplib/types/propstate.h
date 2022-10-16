#ifndef PROPSTATE_H
#define PROPSTATE_H

class PropObject;
class PropLOD;

#include <QVector>
#include <memory>

class PropState
{	
	QVector<std::shared_ptr<PropLOD>> _lods;

public:

	static const QString DEFAULT_NAME;

	PropState();

	void addLOD(std::shared_ptr<PropObject> &prop, double distance);
	int getNumLODs();
	std::shared_ptr<PropLOD> lodByIndex(int index);
	std::shared_ptr<PropObject> getDefaultObject();
	void traceState();
};

#endif // PROPSTATE_H
