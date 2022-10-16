#ifndef ICAMERACONTROLLER_H
#define ICAMERACONTROLLER_H

#include <stdint.h>

class ICameraController
{
public:
	virtual void update(uint32_t, uint32_t) = 0;
	virtual ~ICameraController() {}
};

#endif // ICAMERACONTROLLER_H
