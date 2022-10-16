#ifndef MINEMODELDATA_H
#define MINEMODELDATA_H

class MineModelData
{
public:
	int armingDuration;
	int armedFlashDuration;
	int armedFlashFadeDuration;
	int flashChannelOffset;
	double farRadius = 1500;
	double nearRadius = 500;
};

#endif // MINEMODELDATA_H
