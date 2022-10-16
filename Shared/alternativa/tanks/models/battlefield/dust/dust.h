#ifndef DUST_H
#define DUST_H

#include <memory>
#include <QHash>

class BattlefieldModel;
class TextureAnimation;
class GameCamera;
class BitmapData;
class TankData;
class Tank;
class Track;
class Vector3;


class Dust
{
	BattlefieldModel *battleService;
	double dustSize = 0;
	TextureAnimation *animation;
	QHash<TankData*, double> tanks;
	GameCamera *camera;
	double nearDistance;
	double farDistance;
	double intensity;
	double density;
public:

	bool enabled;


	Dust(BattlefieldModel *battlefieldModel);

	void init(std::shared_ptr<BitmapData> &bitmapData, double farDistance, double nearDistance, double dustSize, double intensity, double density);
	void addTank(TankData *tankData);
	void removeTank(TankData *tankData);
	void update();
	void addTankDust(TankData *tankData, double param2 = 100, double param3 = 0.2);

private:

	void addTrackDust(Track *track, double param2, Vector3 *param3, double param4, double param5, double trackSpeed);
	void createDustParticle(double param1, const Vector3 *param2, const Vector3 *param3, double param4);
};

#endif // DUST_H
