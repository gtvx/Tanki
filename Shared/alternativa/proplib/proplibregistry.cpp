#include "proplibregistry.h"
#include "proplibrary.h"

PropLibRegistry::PropLibRegistry()
{

}

void PropLibRegistry::addLibrary(std::shared_ptr<PropLibrary> &lib)
{
	this->libs[lib->getName()] = lib;
}

void PropLibRegistry::destroy()
{
	QHashIterator<QString, std::shared_ptr<PropLibrary>> i(this->libs);
	while (i.hasNext())
	{
		i.next();
		i.value()->freeMemory();
	}

	this->libs.clear();
}

PropLibrary* PropLibRegistry::getLibrary(const QString &libName)
{
	return this->libs[libName].get();
}

/*
QVector<std::shared_ptr<PropLibrary>> PropLibRegistry::getLibraries()
{
	QVector<PropLibrary*> *res = new QVector<PropLibrary*>;

	for (PropLibrary *lib : this->libs)
		res->append(lib);

	return res;
}
*/
