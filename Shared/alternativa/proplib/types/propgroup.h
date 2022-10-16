#ifndef PROPGROUP_H
#define PROPGROUP_H

#include <QVector>
#include <memory>

class PropData;

class PropGroup
{
public:
	QString name;
	QVector<std::shared_ptr<PropData>> *props;
	QVector<std::shared_ptr<PropGroup>> *groups;

	PropGroup(const QString &name);
	~PropGroup();

	PropData* getPropByName(const QString &propName);
	PropGroup* getGroupByName(const QString &groupName);
	void addProp(std::shared_ptr<PropData> &prop);
	void addGroup(std::shared_ptr<PropGroup> &group);
	void traceGroup();
};

#endif // PROPGROUP_H
