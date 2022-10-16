#ifndef INITPARAMS_H
#define INITPARAMS_H

class FlagData;

class InitParams
{
public:
	FlagData *redFlagData;
	FlagData *blueFlagData;

	InitParams(FlagData *redFlagData, FlagData *blueFlagData);
};

#endif // INITPARAMS_H
