#include "randomgenerator.h"
#include <QRandomGenerator>

static QRandomGenerator r;

double RandomGenerator::random()
{
	return r.generateDouble();
}
