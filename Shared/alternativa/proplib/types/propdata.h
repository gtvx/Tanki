#ifndef PROPDATA_H
#define PROPDATA_H

#include <QHash>
#include <memory>

class PropState;

class PropData
{
	QString _name;
	QHash<QString, std::shared_ptr<PropState>> states;

public:

	PropData(const QString &name);
	QString getName();
	void addState(const QString &stateName, std::shared_ptr<PropState> state);
	PropState* getStateByName(const QString &stateName);
	PropState* getDefaultState();
	QString toString();
	void traceProp();
};

#endif // PROPDATA_H
