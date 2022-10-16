#ifndef PROPLIBREGISTRY_H
#define PROPLIBREGISTRY_H

#include <QHash>
#include <memory>

class PropLibrary;

class PropLibRegistry
{
	QHash<QString, std::shared_ptr<PropLibrary>> libs;
public:
	PropLibRegistry();
	void addLibrary(std::shared_ptr<PropLibrary> &lib);
	void destroy();
	PropLibrary* getLibrary(const QString &libName);
	//QVector<PropLibrary*>* getLibraries();
};

#endif // PROPLIBREGISTRY_H
