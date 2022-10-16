#include "tankpreview.h"
#include "alternativa/engine3d/core/object3dcontainer.h"
#include "resource/tanks/tankresource.h"
#include "resource/resourceutil.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/core/face.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/tanks/tank3dpart.h"
#include "alternativa/tanks/tank3d.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "mymath.h"
#include "flash/utils/gettimer.h"
#include "alternativa/engine3d/core/view.h"
#include "service.h"
#include "_global.h"
#include "IMain.h"
#include <QTimer>


static const int windowMargin = 11;

TankPreview::TankPreview(Long *garageBoxId, double rotationSpeed)
{
	(void)garageBoxId;

	object = new QObject;

	lastTime = 0;
	loadedCounter = 0;
	startAngle = -150;
	autoRotationDelay = 10000;
	firstAutoRotation = true;
	first_resize = true;


	this->rotationSpeed = rotationSpeed;
	//this->window = new TankWindow2(400, 300);
	//ILocaleService *localeService = ILocaleService(Main.osgi.getService(ILocaleService));
	//this->window->header = TankWindowHeader.YOUR_TANK;
	//addChild(this->window);
	this->rootContainer = new_Object3DContainer();

	//TankResource
	TankResource *boxResource = ResourceUtil::getResourceModel("garage_box_model");

	//Main.writeVarsToConsoleChannel("TANK PREVIEW", "\tgarageBoxId: %1", garageBoxId);
	//Main.writeVarsToConsoleChannel("TANK PREVIEW", "\tboxResource: %1", boxResource);
	QVector<Mesh*> boxes;

	int numObjects = boxResource->objects->length();
	//(Main.osgi.getService(IConsole) as IConsole).addLine(((("garage :" + numObjects) + " ") + boxResource.id));




	for (int i = 0; i < numObjects; i++)
	{
		Mesh *box = (Mesh*)boxResource->objects->at(i).get();
		if (box == nullptr)
		{
		}
		else
		{
			//Main.writeVarsToConsoleChannel("TEST", "TankPreview::TankPreview() box texture=%1", "E");
#ifdef GRAPHICS
			TextureMaterial *material = (TextureMaterial*)box->faceList()->material.get();
			ImageResource *image = ResourceUtil::getResourceImage("garage_box_img");
			if (image == nullptr)
			{
				qDebug("error TankPreview()");
				return;
			}
			else
			{
				material->setTexture(image->bitmapData->data);
				boxes.append(box);
			}
#else
			boxes.append(box);
#endif
		}
	}

	this->autoRotationTimer = new QTimer();
	this->autoRotationTimer->setInterval(this->autoRotationDelay);
	this->autoRotationTimer->setSingleShot(true);
    QObject::connect(this->autoRotationTimer, &QTimer::timeout, object, [this] { this->start(); });


	this->slowdownTimer = new QTimer();
	this->slowdownTimer->setInterval(20);
    QObject::connect(this->slowdownTimer, &QTimer::timeout, object, [this] { this->slowDown(); });

	this->timer = new QTimer();
	this->timer->setInterval(50);


	this->rootContainer->addChild(boxes[0]);
	this->rootContainer->addChild(boxes[2]);
	this->rootContainer->addChild(boxes[1]);
	this->rootContainer->addChild(boxes[3]);
	this->tank = new_Tank3D(nullptr, nullptr, nullptr);
	this->rootContainer->addChild(this->tank.get());
	//this->tank->matrix.appendTranslation(-17, 0, 0); ?
	this->camera = new_GameCamera();
	this->camera->view = new View(100, 100, 500, 500, false);
	this->camera->view->hideLogo();
	this->camera->useShadowMap = true;
	//addChild(this->camera.view);
	//addChild(this->overlay);
	//this->overlay->x = 0;
	//this->overlay->y = 9;
	//this->overlay->width = 1500;
	//this->overlay->height = 1300;
	//this->overlay->graphics.clear();
	this->cameraContainer = new_Object3DContainer();

	this->rootContainer->addChild(this->cameraContainer.get());
	this->cameraContainer->addChild(this->camera.get());



	this->camera->position.z = -740;
	this->cameraContainer->rotation.x = (-135 * MyMath::PI) / 180;
	this->cameraContainer->rotation.z = (this->startAngle * MyMath::PI) / 180;
	//this->inner = new TankWindowInner(0, 0, TankWindowInner.TRANSPARENT);
	//addChild(this->inner);
	//this->inner->mouseEnabled = true;
	//this->resize(400, 300);
	//this->autoRotationTimer = new Timer(this->autoRotationDelay, 1);
	//this->autoRotationTimer.addEventListener(TimerEvent.TIMER_COMPLETE, this->start);
	//this->timer = new Timer(50);
	//this->slowdownTimer = new Timer(20, 1000000);
	//this->slowdownTimer->addEventListener(TimerEvent.TIMER, this->slowDown);
	//this->inner->addEventListener(MouseEvent.MOUSE_DOWN, this->onMouseDown);
	//Main.stage.addEventListener(Event.ENTER_FRAME, this->onRender);

	this->renderTimer = new QTimer();
	this->renderTimer->setInterval(20);
    QObject::connect(this->renderTimer, &QTimer::timeout, object, [this] { this->onRender(); });
	this->renderTimer->start();

	this->start();
}

TankPreview::~TankPreview()
{
	this->camera->view->dispose();
	this->camera = nullptr;

	delete this->autoRotationTimer;
	delete this->slowdownTimer;
	delete this->timer;
	delete this->renderTimer;
	delete object;
	//object->deleteLater();
}

void TankPreview::setHull(const QString &hull_)
{
	QString hull = hull_;

	if (hull.indexOf("HD_") != -1)
	{
		hull = hull.replace("HD_", "");
	}

	Tank3DPart *hullPart = new Tank3DPart();
#ifdef GRAPHICS
	hullPart->details = ResourceUtil::getResourceImage(hull + "_details")->bitmapData->data;
	hullPart->lightmap = ResourceUtil::getResourceImage(hull + "_lightmap")->bitmapData->data;
#endif

	hullPart->mesh = ResourceUtil::getResourceModel(hull)->mesh;
	hullPart->turretMountPoint = ResourceUtil::getResourceModel(hull)->turretMount;
	this->tank->setHull(hullPart);

	if (this->loadedCounter < 3)
		this->loadedCounter++;

	if (this->loadedCounter == 3)
	{
		if ((((this->firstAutoRotation) && (!(this->timer->isActive()))) && (!(this->slowdownTimer->isActive()))))
		{
			this->start();
		}
		this->camera->render();
	}
}

void TankPreview::setTurret(const QString &turret_)
{
	QString turret = turret_;

	Tank3DPart *turretPart = new Tank3DPart();

#ifdef GRAPHICS
	ImageResource *imageResource_details = ResourceUtil::getResourceImage(turret + "_details");
	ImageResource *imageResource_lightmap = ResourceUtil::getResourceImage(turret + "_lightmap");

	imageResource_details->load();
	imageResource_lightmap->load();

	turretPart->details = imageResource_details->bitmapData->data;
	turretPart->lightmap = imageResource_lightmap->bitmapData->data;
#endif

	//turretPart->details = ResourceUtil::getResourceImage(turret + "_details")->bitmapData->data;
	//turretPart->lightmap = ResourceUtil::getResourceImage(turret + "_lightmap")->bitmapData->data;


	if (turret.indexOf("HD_") != -1)
		turret = turret.replace("HD_", "");

	turretPart->mesh = ResourceUtil::getResourceModel(turret)->mesh;
	turretPart->turretMountPoint = ResourceUtil::getResourceModel(turret)->turretMount;
	this->tank->setTurret(turretPart);

	if (this->loadedCounter < 3)
		this->loadedCounter++;

	if (this->loadedCounter == 3)
	{
		if ((((this->firstAutoRotation) && (!(this->timer->isActive()))) && (!(this->slowdownTimer->isActive()))))
		{
			this->start();
			this->camera->addShadow(this->tank->shadow);
		}
		this->camera->render();
	}
}

void TankPreview::setColorMap(ImageResource *map)
{
	this->tank->setColorMap(map);

	if (this->loadedCounter < 3)
		this->loadedCounter++;

	if (this->loadedCounter == 3)
	{
		if ((((this->firstAutoRotation) && (!(this->timer->isActive()))) && (!(this->slowdownTimer->isActive()))))
		{
			this->start();
		}
		this->camera->render();
	}
}

void TankPreview::start()
{
	if (this->loadedCounter < 3)
	{
		//this->autoRotationTimer->reset();
		this->autoRotationTimer->start();
	}
	else
	{
		this->firstAutoRotation = false;
        QObject::connect(this->timer, &QTimer::timeout, object, [this] { this->onTimer(); });
		//this->timer->reset();
		this->lastTime = getTimer();
		this->timer->start();
	}
}

void TankPreview::dispose()
{
	delete this;
}



void TankPreview::onTimer()
{
	int time = this->lastTime;
	this->lastTime = getTimer();
	this->cameraContainer->rotation.z -= ((this->rotationSpeed * (this->lastTime - time)) * 0.001) * (MyMath::PI / 180);
}



void TankPreview::slowDown()
{
	this->cameraContainer->rotation.z -= this->rate * 0.01;
	this->camera->render();
	this->rate = this->rate * MyMath::exp(-0.02);

	if (MyMath::abs(this->rate) < 0.1)
	{
		this->slowdownTimer->stop();
		//this->autoRotationTimer->reset();
		this->autoRotationTimer->start();
	}
}

void TankPreview::onRender()
{
	if (Service::getIMain()->renderEnable())
		this->camera->render();
}
