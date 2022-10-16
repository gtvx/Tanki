#ifndef DECALENTRY_H
#define DECALENTRY_H

#include <memory>

class Decal;


class DecalEntry
{
public:
	std::shared_ptr<Decal> decal;
	int startTime;

	DecalEntry(std::shared_ptr<Decal> &decal, int startTime)
	{
		this->decal = decal;
		this->startTime = startTime;
	}
};


#endif // DECALENTRY_H
