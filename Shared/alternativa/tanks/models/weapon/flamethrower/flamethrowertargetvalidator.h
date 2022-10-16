#ifndef FLAMETHROWERTARGETVALIDATOR_H
#define FLAMETHROWERTARGETVALIDATOR_H

#include "alternativa/tanks/models/weapon/shared/ITargetValidator.h"

class FlamethrowerTargetValidator : public ITargetValidator
{
public:
	bool isValidTarget(Body *targetBody) override;
	virtual ~FlamethrowerTargetValidator();
	void destroy() override;
};

#endif // FLAMETHROWERTARGETVALIDATOR_H
