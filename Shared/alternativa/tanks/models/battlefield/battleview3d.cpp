#include "battleview3d.h"
#include "alternativa/engine3d/core/view.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/engine3d/core/object3dcontainer.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/object3dcontainerproxy.h"
#include "alternativa/engine3d/core/object3dcontainer.h"
#include "decals/RotationState.h"
#include "decals/decalfactory.h"
#include "alternativa/engine3d/containers/kdcontainer.h"
#include "alternativa/engine3d/objects/decal.h"
#include "decals/fadingdecalsrenderer.h"
#include "battlefieldmodel.h"
#include "_global.h"
#include "service.h"
#include "alternativa/tanks/model/panel/panelmodel.h"
#include "mymath.h"
#include "ambientshadows.h"


#define MAX_TEMPORARY_DECALS 20
#define DECAL_FADING_TIME_MS 20000


BattleView3D::BattleView3D(bool showAxisIndicator, ICollisionDetector *collisionDetector, BattlefieldModel *bs)
{
	(void)showAxisIndicator;
	(void)collisionDetector;

	this->_camera = new_GameCamera();
	this->camera = this->_camera.get();


	this->camera->nearClipping = 10;
	this->camera->farClipping = 50000;
	this->camera->view = new View(0, 0, 400, 400);
	//this->camera->view->hideLogo();
	//this->camera->view->focusRect = false;
	this->camera->softTransparency = true;
	this->camera->softAttenuation = 130;
	this->camera->softTransparencyStrength = 1;
	this->camera->position.z += 10000;
	this->camera->rotation.x = MyMath::PI;
	this->camera->debug = false;


	this->mapContainerProxy = std::make_shared<Object3DContainerProxy>(nullptr);

	this->rootContainer = new_Object3DContainer();
	this->rootContainer->setName("rootContainer");

	this->skyboxContainer = new_Object3DContainer();
	this->skyboxContainer->setName("skyboxContainer");

	this->mainContainer = new_Object3DContainer();
	this->mainContainer->setName("mainContainer");

	this->frontContainer = new_Object3DContainer();
	this->frontContainer->setName("frontContainer");

	this->rootContainer->addChild(this->skyboxContainer.get());
	this->rootContainer->addChild(this->mainContainer.get());
	this->rootContainer->addChild(this->frontContainer.get());

	this->frontContainerProxy = std::make_shared<Object3DContainerProxy>(this->frontContainer);
	this->rootContainer->addChild(this->camera);
#ifdef GRAPHICS
	this->decalFactory = new DecalFactory(collisionDetector);
#endif
	this->fadingDecalRenderer = bs->fadingDecalRenderer;

	this->ambientShadows = new AmbientShadows(this->camera);
}


BattleView3D::~BattleView3D()
{
	this->camera->view->dispose();
	delete this->ambientShadows;
}


void BattleView3D::enableAmbientShadows(bool enable)
{
	if (enable)
	{
		this->ambientShadows->enable();
	}
	else
	{
		this->ambientShadows->disable();
	}
}

void BattleView3D::addAmbientShadow(Shadow *shadow)
{
	this->ambientShadows->add(shadow);
}

void BattleView3D::removeAmbientShadow(Shadow *shadow)
{
	this->ambientShadows->remove(shadow);
}

void BattleView3D::enableFog()
{
	this->camera->fogNear = 5000;
	this->camera->fogFar = 10000;
	this->camera->fogStrength = 1;
	this->camera->fogAlpha = 0.35;
}

void BattleView3D::disableFog()
{
	this->camera->fogStrength = 0;
}

void BattleView3D::update()
{
	//if (this->_showAxisIndicator)
	//this->axisIndicator->update(this->camera);

	Service::getPanelModel()->draw(this->stage);

	this->camera->render();
}

Scene3DContainer* BattleView3D::getFrontContainer()
{
	return this->frontContainerProxy.get();
}

Scene3DContainer* BattleView3D::getMapContainer()
{
	return this->mapContainerProxy.get();
}

void BattleView3D::setStage(Stage *stage)
{
	this->stage = stage;
}

void BattleView3D::resize(double w, double h)
{
	this->camera->view->setSize(w, h);
	//if (this->_showAxisIndicator)
	//this->axisIndicator->y = (((h + this->camera.view.height) >> 1) - (2 * this->axisIndicator.size));

	this->camera->updateFov();
	//this->container->x = (this->stage.stageWidth - this->camera.view.width) >> 1;
	//this->container->y = (this->stage.stageHeight - this->camera.view.height) >> 1;
}


void BattleView3D::setMapContainer(std::shared_ptr<Object3DContainer> value)
{
	if (this->mapContainer != nullptr)
	{
		this->mainContainer->removeChild(this->mapContainer.get());
		this->mapContainer = nullptr;
	}

	this->mapContainerProxy->setContainer(value);
	this->mapContainer = value;

	if (this->mapContainer != nullptr)
	{
		this->mainContainer->addChild(this->mapContainer.get());
	}

	this->frontContainer->setName("FRONT CONTAINER");
	this->mapContainer->setName("MAP CONTAINER");
	this->mapContainer->mouseEnabled = true;
	this->mapContainer->mouseChildren = true;
}

void BattleView3D::setSkyBox(Object3D *object)
{
	if (this->skyboxContainer->numChildren() > 0)
	{
		this->skyboxContainer->removeChildAt(0);
	}
	this->skyboxContainer->addChild(object);
}

Object3DContainer* BattleView3D::get_MapContainer()
{
	return this->mapContainer.get();
}


static void clear(Object3DContainer *container)
{
	if (container == nullptr)
		return;

	while (container->numChildren() > 0)
	{
		container->removeChildAt(0);
	}
}

void BattleView3D::clearContainers()
{
	clear(this->rootContainer.get());
	clear(this->mapContainer.get());
	clear(this->skyboxContainer.get());
	clear(this->frontContainer.get());
}



void BattleView3D::addDecal(const Vector3 *hitPosition,
							const Vector3 *muzzlePosition,
							double radius,
							TextureMaterial *material,
							RotationState rotationState)
{
#ifdef GRAPHICS
	if (rotationState == RotationState::NONE)
		rotationState = RotationState::USE_RANDOM_ROTATION;

	KDContainer *container = (KDContainer*)this->mapContainer.get();

	std::shared_ptr<Decal> decal = this->decalFactory->createDecal(hitPosition, muzzlePosition, radius, material, container, rotationState);

	container->addChildAt(decal.get(), 0);
	this->allDecals.append(decal);

	this->temporaryDecals.enqueue(decal);
	if (this->temporaryDecals.size() > MAX_TEMPORARY_DECALS)
	{
		std::shared_ptr<Decal> first = this->temporaryDecals.dequeue();
		this->fadingDecalRenderer->add(first);
	}
#else
	(void)hitPosition;
	(void)muzzlePosition;
	(void)radius;
	(void)material;
	(void)rotationState;
#endif
}


void BattleView3D::removeDecal(Decal *decal)
{
	if (decal == nullptr)
		return;
#ifdef GRAPHICS
	((KDContainer*)mapContainer.get())->removeChild(decal);
	//delete this->allDecals[decal];
	for (int i = 0; i < allDecals.length(); i++)
	{
		if (allDecals.at(i).get() == decal)
		{
			allDecals.removeAt(i);
			break;
		}
	}
#endif
}

void BattleView3D::enableSoftParticles()
{
	this->camera->softTransparency = true;
}

void BattleView3D::disableSoftParticles()
{
	this->camera->softTransparency = false;
}

