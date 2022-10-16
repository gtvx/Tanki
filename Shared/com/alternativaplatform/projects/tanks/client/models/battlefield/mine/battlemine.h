#ifndef BATTLEMINE_H
#define BATTLEMINE_H

#include <QString>

class BattleMine
{
public:
	QString mineId;
	QString ownerId;
	double x;
	double y;
	double z;
	bool activated;
};

#endif // BATTLEMINE_H
