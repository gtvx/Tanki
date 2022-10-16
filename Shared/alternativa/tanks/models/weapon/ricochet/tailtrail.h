#ifndef TAILTRAIL_H
#define TAILTRAIL_H

#include "alternativa/tanks/sfx/simpleplane.h"

class TailTrail : public SimplePlane
{
public:

	static const T *TYPE;
	const T *__type_next;

	TailTrail(const TT &t, double width, double length);
};

std::shared_ptr<TailTrail> new_TailTrail(double width, double length);

#endif // TAILTRAIL_H
