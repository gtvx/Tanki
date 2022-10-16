#ifndef PROPLIBSLOADER_H
#define PROPLIBSLOADER_H

#include "resourceloader.h"
#include <memory>

class PropLibRegistry;
class TaskSequence;
class Config;

class PropLibsLoader : public ResourceLoader
{
	std::shared_ptr<PropLibRegistry> libRegistry;
	TaskSequence *sequence;
public:
	PropLibsLoader(Config *config);
	bool run();
};

#endif // PROPLIBSLOADER_H
