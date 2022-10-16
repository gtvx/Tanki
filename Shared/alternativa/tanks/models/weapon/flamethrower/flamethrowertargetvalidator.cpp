#include "flamethrowertargetvalidator.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"

bool FlamethrowerTargetValidator::isValidTarget(Body *targetBody)
{
	Tank *tank = (Tank*)targetBody;
	return tank->tankData->health > 0;
}

FlamethrowerTargetValidator::~FlamethrowerTargetValidator()
{

}

void FlamethrowerTargetValidator::destroy()
{
	delete this;
}
