#include "lightanimation.h"
#include "lightingeffectrecord.h"
#include "mymath.h"
#include "alternativa/engine3d/lights/spotlight.h"
#include "alternativa/engine3d/lights/omnilight.h"
#include "alternativa/engine3d/lights/tubelight.h"



static double lerpNumber(double param1, double param2, double param3)
{
	return (param1 + ((param2 - param1) * param3));
}

static uint lerpColor(uint param1, uint param2, double param3)
{
	double _loc4_ = (((param1 >> 16) & 0xFF) / 0xFF);
	double _loc5_ = (((param1 >> 8) & 0xFF) / 0xFF);
	double _loc6_ = ((param1 & 0xFF) / 0xFF);
	double _loc7_ = (((param2 >> 16) & 0xFF) / 0xFF);
	double _loc8_ = (((param2 >> 8) & 0xFF) / 0xFF);
	double _loc9_ = ((param2 & 0xFF) / 0xFF);
	int _loc10_ = (lerpNumber(_loc4_, _loc7_, param3) * 0xFF);
	int _loc11_ = (lerpNumber(_loc5_, _loc8_, param3) * 0xFF);
	int _loc12_ = (lerpNumber(_loc6_, _loc9_, param3) * 0xFF);
	return (((_loc10_ << 16) | (_loc11_ << 8)) | _loc12_);
}



LightAnimation::LightAnimation(QVector<LightingEffectRecord *> &frames)
{
	this->frames = frames.length();
	this->intensity.resize(this->frames);
	this->color.resize(this->frames);
	this->attenuationBegin.resize(this->frames);
	this->attenuationEnd.resize(this->frames);
	this->time.resize(this->frames);

	int index = 0;

	while (index < this->frames)
	{
		LightingEffectRecord *l = frames.at(index);
		this->intensity[index] = l->intensity;
		this->color[index] = l->color;
		this->attenuationBegin[index] = l->attenuationBegin;
		this->attenuationEnd[index] = l->attenuationEnd;
		this->time[index] = l->time;
		index++;
	}
}

double LightAnimation::getFrameByTime(double param1)
{
	double _loc2_ = 0;
	if (param1 < this->time.at(this->frames - 1))
	{
		int _loc3_ = 0;
		while (_loc3_ < (this->frames - 1))
		{
			double _loc4_ = this->time.at(_loc3_);
			double _loc5_ = this->time.at(_loc3_ + 1);
			if (((param1 >= _loc4_) && (param1 < _loc5_)))
			{
				double _loc6_ = ((param1 - _loc4_) / (_loc5_ - _loc4_));
				_loc2_ = _loc3_ + _loc6_;
				break;
			}
			_loc3_++;
		}
	}
	else
	{
		_loc2_ = this->frames - 1;
	}
	return _loc2_;
}

int LightAnimation::getFramesCount()
{
	return (this->frames);
}

int LightAnimation::limitFrame(int param1)
{
	return ((param1 < this->frames) ? int(int(param1)) : int(int((this->frames - 1))));
}

void LightAnimation::updateSpotLight(double param1, SpotLight *param2)
{
	int _loc3_ = this->limitFrame(MyMath::floor(param1));
	int _loc4_ = this->limitFrame(MyMath::ceil(param1));
	double _loc5_ = (param1 - _loc3_);
	double _loc6_ = this->intensity.at(_loc3_);
	double _loc7_ = this->intensity.at(_loc4_);
	uint _loc8_ = this->color.at(_loc3_);
	uint _loc9_ = this->color.at(_loc4_);
	double _loc10_ = this->attenuationBegin.at(_loc3_);
	double _loc11_ = this->attenuationBegin.at(_loc4_);
	double _loc12_ = this->attenuationEnd.at(_loc3_);
	double _loc13_ = this->attenuationEnd.at(_loc4_);
	param2->intensity = lerpNumber(_loc6_, _loc7_, _loc5_);
	param2->color = lerpColor(_loc8_, _loc9_, _loc5_);
	param2->attenuationBegin = lerpNumber(_loc10_, _loc11_, _loc5_);
	param2->attenuationEnd = lerpNumber(_loc12_, _loc13_, _loc5_);
}

void LightAnimation::updateOmniLight(double param1, OmniLight *param2)
{
	int _loc3_ = this->limitFrame(MyMath::floor(param1));
	int _loc4_ = this->limitFrame(MyMath::ceil(param1));
	double _loc5_ = (param1 - _loc3_);
	double _loc6_ = this->intensity.at(_loc3_);
	double _loc7_ = this->intensity.at(_loc4_);
	uint _loc8_ = this->color.at(_loc3_);
	uint _loc9_ = this->color.at(_loc4_);
	double _loc10_ = this->attenuationBegin.at(_loc3_);
	double _loc11_ = this->attenuationBegin.at(_loc4_);
	double _loc12_ = this->attenuationEnd.at(_loc3_);
	double _loc13_ = this->attenuationEnd.at(_loc4_);
	param2->intensity = lerpNumber(_loc6_, _loc7_, _loc5_);
	param2->color = lerpColor(_loc8_, _loc9_, _loc5_);
	param2->attenuationBegin = lerpNumber(_loc10_, _loc11_, _loc5_);
	param2->attenuationEnd = lerpNumber(_loc12_, _loc13_, _loc5_);
}

void LightAnimation::updateTubeLight(double param1, TubeLight *param2)
{
	int _loc3_ = this->limitFrame(MyMath::floor(param1));
	int _loc4_ = this->limitFrame(MyMath::ceil(param1));
	double _loc5_ = (param1 - _loc3_);
	double _loc6_ = this->intensity.at(_loc3_);
	double _loc7_ = this->intensity.at(_loc4_);
	uint _loc8_ = this->color.at(_loc3_);
	uint _loc9_ = this->color.at(_loc4_);
	double _loc10_ = this->attenuationBegin.at(_loc3_);
	double _loc11_ = this->attenuationBegin.at(_loc4_);
	double _loc12_ = this->attenuationEnd.at(_loc3_);
	double _loc13_ = this->attenuationEnd.at(_loc4_);
	param2->intensity = lerpNumber(_loc6_, _loc7_, _loc5_);
	param2->color = lerpColor(_loc8_, _loc9_, _loc5_);
	param2->attenuationBegin = lerpNumber(_loc10_, _loc11_, _loc5_);
	param2->attenuationEnd = lerpNumber(_loc12_, _loc13_, _loc5_);
}

void LightAnimation::updateByTime(Light3D *param1, int param2, int param3)
{
	double _loc4_ = 1;
	if (((param3 > 0) && (this->frames > 0)))
	{
		_loc4_ = double((int)this->time.at(this->frames - 1)) / param3;
	}
	double _loc5_ = this->getFrameByTime((param2 * _loc4_));
	this->updateByFrame(param1, _loc5_);
}

void LightAnimation::updateByFrame(Light3D *param1, double param2)
{
	if (param1->isType(OmniLight::TYPE))
	{
		this->updateOmniLight(param2, (OmniLight*)param1);
	}
	else
	{
		if (param1->isType(SpotLight::TYPE))
		{
			this->updateSpotLight(param2, (SpotLight*)(param1));
		}
		else
		{
			if (param1->isType(TubeLight::TYPE))
			{
				this->updateTubeLight(param2, (TubeLight*)(param1));
			}
		}
	}
	param1->calculateBounds();
}

int LightAnimation::getLiveTime()
{
	return this->time.at(this->frames - 1);
}
