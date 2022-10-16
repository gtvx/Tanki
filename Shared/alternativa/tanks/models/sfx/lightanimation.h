#ifndef LIGHTANIMATION_H
#define LIGHTANIMATION_H

#include <QVector>

class LightingEffectRecord;
class SpotLight;
class OmniLight;
class TubeLight;
class Light3D;


class LightAnimation
{
	int frames;
	QVector<uint> time;
	QVector<double> intensity;
	QVector<uint> color;
	QVector<double> attenuationBegin;
	QVector<double> attenuationEnd;
public:

	LightAnimation(QVector<LightingEffectRecord*> &frames);

	double getFrameByTime(double param1);
	int getFramesCount();

private:
	int limitFrame(int param1);
	void updateSpotLight(double param1, SpotLight *param2);
	void updateOmniLight(double param1, OmniLight *param2);
	void updateTubeLight(double param1, TubeLight *param2);

public:
	void updateByTime(Light3D *param1, int param2, int param3 = -1);

private:
	void updateByFrame(Light3D *param1, double param2);

public:
	int getLiveTime();
};

#endif // LIGHTANIMATION_H
