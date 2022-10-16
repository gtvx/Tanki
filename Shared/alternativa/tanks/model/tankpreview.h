#ifndef TANKPREVIEW_H
#define TANKPREVIEW_H


#include <stdint.h>
#include <memory>


class TankWindow2;
class TankWindowInner;
class Object3DContainer;
class GameCamera;
class QTimer;
class Tank3D;
class Long;
class ImageResource;
class QString;
class QObject;


class TankPreview
{
public:
	TankWindow2 *window;
	TankWindowInner *inner;
	std::shared_ptr<Object3DContainer> rootContainer;
	std::shared_ptr<Object3DContainer> cameraContainer;
	std::shared_ptr<GameCamera> camera;
	QTimer *timer;
	std::shared_ptr<Tank3D> tank;
	QTimer *slowdownTimer;
	QTimer *autoRotationTimer;
	QTimer *renderTimer;
	QObject *object;



	double rate;
	double startAngle;
	double holdAngle;
	double rotationSpeed;

	int lastTime;
	int loadedCounter;
	int holdMouseX;
	int lastMouseX;
	int prelastMouseX;
	int autoRotationDelay;
	uint32_t resetRateInt;

	bool firstAutoRotation;
	bool first_resize;

public:
	//public var overlay:Shape = new Shape();

	TankPreview(Long *garageBoxId, double rotationSpeed = 5);
	~TankPreview();

	void setHull(const QString &hull);
	void setTurret(const QString &turret);
	void setColorMap(ImageResource *map);
	void start();

	void dispose();

private:
	void slowDown();
	void onTimer();
	void onRender();
};

#endif // TANKPREVIEW_H
