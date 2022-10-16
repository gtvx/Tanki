#include "propgroup.h"
#include "propdata.h"


PropGroup::PropGroup(const QString &name)
{
	this->name = name;
	this->props = nullptr;
	this->groups = nullptr;
}

PropGroup::~PropGroup()
{
	if (props != nullptr)
		delete props;

	if (groups != nullptr)
		delete groups;
}

PropData *PropGroup::getPropByName(const QString &propName)
{
	if (this->props == nullptr)
	{
		return nullptr;
	}

	for (std::shared_ptr<PropData> &prop : *props)
	{
		if (prop->getName() == propName)
		{
			return prop.get();
		}
	}

	return nullptr;
}

PropGroup *PropGroup::getGroupByName(const QString &groupName)
{
	if (this->groups == nullptr)
	{
		return nullptr;
	}

	for (std::shared_ptr<PropGroup> &group : *this->groups)
	{
		if (group->name == groupName)
		{
			return group.get();
		}
	}

	return nullptr;
}

void PropGroup::addProp(std::shared_ptr<PropData> &prop)
{
	if (this->props == nullptr)
	{
		this->props = new QVector<std::shared_ptr<PropData>>;
	}
	this->props->append(prop);
}

void PropGroup::addGroup(std::shared_ptr<PropGroup> &group)
{
	if (this->groups == nullptr)
	{
		this->groups = new QVector<std::shared_ptr<PropGroup>>;
	}
	this->groups->append(group);
}

void PropGroup::traceGroup()
{
	if (this->groups != nullptr)
	{
		for (std::shared_ptr<PropGroup> &group : *this->groups)
		{
			group->traceGroup();
		}
	}

	if (this->props != nullptr)
	{
		for(std::shared_ptr<PropData> &prop : *this->props)
		{
			prop->traceProp();
		}
	}
}
