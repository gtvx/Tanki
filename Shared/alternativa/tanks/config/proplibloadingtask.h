#ifndef PROPLIBLOADINGTASK_H
#define PROPLIBLOADINGTASK_H

#include <QString>
#include <memory>

class PropLibRegistry;

class PropLibLoadingTask
{
	QString url;
	std::shared_ptr<PropLibRegistry> libRegistry;
	//CacheURLLoader loader;
public:
	PropLibLoadingTask(const QString &url, std::shared_ptr<PropLibRegistry> &libRegistry);
	bool run();
};

#endif // PROPLIBLOADINGTASK_H
