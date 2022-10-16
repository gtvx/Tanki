#include "tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/math/vector3.h"
#include "service.h"
#include "tankskinturret.h"
#include "tankskinhull.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "_global.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "trackskin.h"
#include "alternativa/tanks/materials/trackmaterial.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/vertex.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "alternativa/tanks/materials/animatedpaintmaterial.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "flash/geom/colortransform.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/engine3d/core/shadow.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include <QHash>


class SkinStateMaterials
{
public:

	BitmapData *coloring;
	Material *hullMaterial;
	Material *turretMaterial;


	SkinStateMaterials(BitmapData *coloring, Material *hullMaterial, Material *turretMaterial)
	{
		this->coloring = coloring;
		this->hullMaterial = hullMaterial;
		this->turretMaterial = turretMaterial;
	}

	void release(bool fully = false)
	{
		if (this->hullMaterial->isType(TextureMaterial::TYPE) && fully)
		{
			((TextureMaterial*)this->hullMaterial)->dispose();
			this->hullMaterial = nullptr;
		}
		if (this->turretMaterial->isType(TextureMaterial::TYPE) && fully)
		{
			((TextureMaterial*)this->turretMaterial)->dispose();
			this->turretMaterial = nullptr;
		}
		this->coloring = nullptr;
		this->hullMaterial = nullptr;
		this->turretMaterial = nullptr;
	}
};

class TankSkinPrivate
{
public:
	QString detailsID;
	QString lmHullID;
	QString dtHullID;
	QString lmTurrID;
	QString dtTurrID;
};

enum
{
	STATE_NORMAL = 1,
	STATE_DEAD = 2,
};


static QHash<QString, std::shared_ptr<BitmapData>> compositions;


void TankSkin::_clear()
{
	compositions.clear();
}

#ifdef GRAPHICS
static std::shared_ptr<BitmapData> createTexture(BitmapData *colormap, BitmapData *lightmap, BitmapData *details)
{
	std::shared_ptr<BitmapData> texture = std::make_shared<BitmapData>(lightmap->width(), lightmap->height(), false, 0);
	//shape = new Shape();
	//shape.graphics.beginBitmapFill(colormap);
	//shape.graphics.drawRect(0, 0, lightmap.width, lightmap.height);
	//texture->draw(shape);
	//texture->draw(lightmap, null, null, BlendMode.HARDLIGHT);
	//texture->draw(details);

	texture->beginBitmapFill(colormap);
	texture->draw(lightmap, nullptr, nullptr, BlendMode::HARDLIGHT);
	texture->draw(details);

	return texture;
}

static std::shared_ptr<SkinStateMaterials> createStateMaterials(ImageResource *coloring,
																ITextureMaterialRegistry *textureMaterialRegistry,
																const QString &ligthmapHull,
																const QString &detailsHull,
																const QString &ligthmapTurret,
																const QString &detailsTurret,
																bool isDeadTexture = false)
{
	//bool useMipMapping = true; //IBattleSettings(Main.osgi.getService(IBattleSettings)).enableMipMapping;
	const QString idCompositionHull = (((((((coloring->id + "_") + ligthmapHull) + "_") + detailsHull) + "_") + "_") + QString::number((int)isDeadTexture));
	const QString idCompositionTurret = (((((((coloring->id + "_") + ligthmapTurret) + "_") + detailsTurret) + "_") + "_") + QString::number((int)isDeadTexture));
	std::shared_ptr<BitmapData> hullComposition = compositions[idCompositionHull];


	if (hullComposition == nullptr)
	{
		BitmapData *lh = ResourceUtil::getResourceImage(ligthmapHull)->bitmapData->data.get();
		BitmapData *dh = ResourceUtil::getResourceImage(detailsHull)->bitmapData->data.get();

		hullComposition = ((isDeadTexture) ?
							   ::createTexture(ResourceUtil::getResourceImage("deadTank")->bitmapData->data.get(), lh, dh) :
							   ::createTexture(coloring->bitmapData->data.get(), lh, dh));

		compositions[idCompositionHull] = hullComposition;
	}


	std::shared_ptr<BitmapData> turretComposition = compositions[idCompositionTurret];
	if (turretComposition == nullptr)
	{
		BitmapData *lt = ResourceUtil::getResourceImage(ligthmapTurret)->bitmapData->data.get();
		BitmapData *dt = ResourceUtil::getResourceImage(detailsTurret)->bitmapData->data.get();
		turretComposition = ((isDeadTexture) ?
								 ::createTexture(ResourceUtil::getResourceImage("deadTank")->bitmapData->data.get(), lt, dt) :
								 ::createTexture(coloring->bitmapData->data.get(), lt, dt));

		compositions[idCompositionTurret] = turretComposition;
	}





	Material *turretMaterial = nullptr;
	Material *hullMaterial = nullptr;

	if (((coloring->animatedMaterial) && (!(isDeadTexture))))
	{
		turretMaterial = textureMaterialRegistry->getAnimatedPaint(coloring,
																   ResourceUtil::getResourceImage(ligthmapTurret)->bitmapData->data,
																   ResourceUtil::getResourceImage(detailsTurret)->bitmapData->data,
																   detailsTurret);


		hullMaterial = textureMaterialRegistry->getAnimatedPaint(coloring,
																 ResourceUtil::getResourceImage(ligthmapHull)->bitmapData->data,
																 ResourceUtil::getResourceImage(detailsHull)->bitmapData->data,
																 detailsHull);
	}
	else
	{
		if (!isDeadTexture)
		{
			turretMaterial = textureMaterialRegistry->getMaterial(MaterialType::TANK, turretComposition, 1);
			hullMaterial = textureMaterialRegistry->getMaterial(MaterialType::TANK, hullComposition, 1);
		}
	}


	if (isDeadTexture)
	{
		hullMaterial = textureMaterialRegistry->getMaterial(MaterialType::TANK, hullComposition, 1);
		turretMaterial = textureMaterialRegistry->getMaterial(MaterialType::TANK, turretComposition, 1);
	}


	/*
	if (this->bfModel.blacklist.indexOf((turretMaterial as TextureMaterial).getTextureResource()) == -1)
	{
		this->bfModel->blacklist.push((turretMaterial as TextureMaterial).getTextureResource());
	}

	if (this->bfModel.blacklist.indexOf((hullMaterial as TextureMaterial).getTextureResource()) == -1)
	{
		this->bfModel.blacklist.push((hullMaterial as TextureMaterial).getTextureResource());
	}
	*/

	return std::make_shared<SkinStateMaterials>(coloring->bitmapData->data.get(), hullMaterial, turretMaterial);
}

#endif





TankSkin::TankSkin(TankSkinHull *hull, TankSkinTurret *turret, ImageResource *normalColoring,
				   const QString &lightmapHullId, const QString &detailsHullId, const QString &lightmapTurretid,
				   const QString &detailsTurretId, ITextureMaterialRegistry *textureMaterialRegistry)
{
	(void)textureMaterialRegistry;

	d = new TankSkinPrivate;

	this->container = nullptr;
	this->colorTransform = std::make_shared<ColorTransform>();
	this->resetColorTransform();

#ifdef GRAPHICS
	this->textureRegistry = Service::getIMaterialRegistry()->textureMaterialRegistry();
#else
	this->textureRegistry = nullptr;
#endif

	this->skinState = 0;
	this->turretDirection = 0;

	this->bfModel = (BattlefieldModel*)Service::getIBattleField();
	this->_hullMesh = this->initHull(hull);
	this->hullMesh = this->_hullMesh.get();
	this->_turretMesh = this->initTurret(turret);
	this->turretMesh = this->_turretMesh.get();
	this->d->detailsID = detailsHullId;
	this->coloring = normalColoring;
	this->registry = textureRegistry;
	this->d->lmHullID = lightmapHullId;
	this->d->dtHullID = detailsHullId;
	this->d->lmTurrID = lightmapTurretid;
	this->d->dtTurrID = detailsTurretId;

	this->leftTrackSkin = nullptr;
	this->rightTrackSkin = nullptr;

	this->createTrackSkins();



#ifdef GRAPHICS
	this->normalMaterials = createStateMaterials(normalColoring, textureRegistry, lightmapHullId, detailsHullId, lightmapTurretid, detailsTurretId);
	this->deadMaterials = createStateMaterials(normalColoring, textureRegistry, lightmapHullId, detailsHullId, lightmapTurretid, detailsTurretId, true);

	this->shadow = new Shadow(128, 6, 100, 5000, 10000, 516, 1);
	this->shadow->offset = 100;
	this->shadow->backFadeRange = 100;
	this->shadow->addCaster(this->hullMesh);
	this->shadow->addCaster(this->turretMesh);
#else
	this->shadow = nullptr;
#endif


}

TankSkin::~TankSkin()
{
	this->hullMesh->destroy();
	this->turretMesh->destroy();
	delete turretDescriptor;
	delete hullDescriptor;
	this->hullMesh = nullptr;
	this->turretMesh = nullptr;
	if (leftTrackSkin != nullptr)
		delete leftTrackSkin;
	if (rightTrackSkin != nullptr)
		delete rightTrackSkin;
	delete this->d;

#ifdef GRAPHICS
	delete this->shadow;
#endif
}

void TankSkin::updateHullTransform(const Vector3 *position, const Vector3 *rotation)
{
	_function_name("TankSkin::updateHullTransform");
	if (this->hullDescriptor != nullptr)
	{
		this->hullMesh->position.copy(position);
		this->hullMesh->rotation.copy(rotation);
	}
}

void TankSkin::setNormalState()
{
	_function_name("TankSkin::setNormalState");
	this->skinState = STATE_NORMAL;
#ifdef GRAPHICS
	this->hullMesh->setMaterialToAllFaces(this->normalMaterials->hullMaterial);
	this->turretMesh->setMaterialToAllFaces(this->normalMaterials->turretMaterial);
#endif
}

void TankSkin::setDeadState()
{
	_function_name("TankSkin::setDeadState");
	this->skinState = STATE_DEAD;
#ifdef GRAPHICS
	this->hullMesh->setMaterialToAllFaces(this->deadMaterials->hullMaterial);
	this->turretMesh->setMaterialToAllFaces(this->deadMaterials->turretMaterial);
#endif
}

std::shared_ptr<Mesh> TankSkin::initHull(TankSkinHull *hullDescriptor)
{
	this->hullDescriptor = hullDescriptor;
	return this->createMesh(hullDescriptor->mesh);
}

std::shared_ptr<Mesh> TankSkin::initTurret(TankSkinTurret *turretDescriptor)
{
	this->turretDescriptor = turretDescriptor;
	return this->createMesh(turretDescriptor->mesh);
}

std::shared_ptr<Mesh> TankSkin::createMesh(std::shared_ptr<Mesh> &referenceMesh)
{
	std::shared_ptr<Mesh> mesh = Mesh_clone(referenceMesh);
	mesh->colorTransform = this->colorTransform;
	return mesh;
}

void TankSkin::updateTransform(Vector3 *pos, Quaternion *orientation)
{
	(void)pos;
	(void)orientation;
	this->updateTurretTransform();
}

void TankSkin::updateTurretTransform()
{
	if (this->hullDescriptor != nullptr && this->turretDescriptor != nullptr)
	{
		Matrix4 hullMatrix;
		hullMatrix.setMatrix(&this->hullMesh->position, &this->hullMesh->rotation);
		const Vector3 &turretMountPoint = this->hullDescriptor->turretMountPoint;
		Matrix4 turretMatrix;
		turretMatrix.setMatrix(turretMountPoint.x, turretMountPoint.y, turretMountPoint.z + 1., 0., 0., this->turretDirection);
		turretMatrix.append(&hullMatrix);
		Vector3 vector;
		turretMatrix.getEulerAngles(&vector);
		this->turretMesh->position.x = turretMatrix.d;
		this->turretMesh->position.y = turretMatrix.h;
		this->turretMesh->position.z = turretMatrix.l;
		this->turretMesh->rotation.copy(&vector);
	}
}

void TankSkin::dispose()
{

}

#ifdef GRAPHICS
static QVector<std::shared_ptr<Face>> getFaces(Mesh *mesh)
{
	QVector<std::shared_ptr<Face>> faces;

	std::shared_ptr<Face> face = mesh->faceList();

	while (face != nullptr)
	{
		faces.append(face);
		face = face->next;
	}

	return faces;
}
#endif

void TankSkin::createTrackSkins()
{
#ifdef GRAPHICS
	this->leftTrackSkin = new TrackSkin();
	this->rightTrackSkin = new TrackSkin();

	QVector<std::shared_ptr<Face>> faces = getFaces(this->hullMesh);

	for (std::shared_ptr<Face> &face : faces)
	{
		if (/*(face->id == "track" || */ face->material->getName() == "track")
		{
			this->addFaceToTrackSkin(face);
		}
	}

	this->leftTrackSkin->init();
	this->rightTrackSkin->init();
	std::shared_ptr<BitmapData> _details = std::make_shared<BitmapData>();
	_details->copy(ResourceUtil::getResourceImage(this->d->detailsID)->bitmapData->data.get());
	this->leftTrackSkin->setMaterial(new_TrackMaterial(_details));
	this->rightTrackSkin->setMaterial(new_TrackMaterial(_details));
#endif
}


void TankSkin::addFaceToTrackSkin(std::shared_ptr<Face> face)
{
#ifdef GRAPHICS
	QVector<std::shared_ptr<Vertex>> *faces = face->getVertices();
	std::shared_ptr<Vertex> vertex = faces->at(0);
	if (vertex->x < 0)
	{
		this->leftTrackSkin->addFace(face);
	}
	else
	{
		this->rightTrackSkin->addFace(face);
	}
	delete faces;
#else
	(void)face;
#endif
}



void TankSkin::resetColorTransform()
{
	this->colorTransformOffset = 0;
	this->targetColorTransformOffset = 0;
	this->colorTransform->redMultiplier = 1;
	this->colorTransform->greenMultiplier = 1;
	this->colorTransform->blueMultiplier = 1;
	this->colorTransform->redOffset = 0;
	this->colorTransform->greenOffset = 0;
	this->colorTransform->blueOffset = 0;
}


void TankSkin::addToContainer(Scene3DContainer *container)
{
	if (this->container == nullptr)
	{
		this->container = container;
	}
	container->addChild(this->hullMesh);
	container->addChild(this->turretMesh);
	if (this->shadow != nullptr)
	{
		this->shadow->alpha = 1;
		this->bfModel->bfData->viewport->addAmbientShadow(this->shadow);
	}
}

void TankSkin::removeFromContainer()
{
	this->container->removeChild(this->hullMesh);
	this->container->removeChild(this->turretMesh);
	this->hullMesh->removeFromParent();
	this->turretMesh->removeFromParent();
	if (this->shadow != nullptr)
	{
		this->bfModel->bfData->viewport->removeAmbientShadow(this->shadow);
	}
}

void TankSkin::updateTracks(double left, double right)
{
#ifdef GRAPHICS
	this->leftTrackSkin->move(left);
	this->rightTrackSkin->move(right);
#else
	(void)left;
	(void)right;
#endif
}

static const double colorTranformPercentSpeed = 0.4;
static const ColorTransform *MAX_COLOR_TRANSFORM = new ColorTransform(0.8, 1, 1.2, 1, 40, 60, 70);

void TankSkin::updateColorTransform(double dt)
{
	if (this->colorTransformOffset == this->targetColorTransformOffset)
	{
		return;
	}

	if (this->colorTransformOffset > this->targetColorTransformOffset)
	{
		this->colorTransformOffset = this->colorTransformOffset - (colorTranformPercentSpeed * dt);
		if (this->colorTransformOffset < this->targetColorTransformOffset)
		{
			this->colorTransformOffset = this->targetColorTransformOffset;
		}
	}
	else
	{
		this->colorTransformOffset = this->colorTransformOffset + (colorTranformPercentSpeed * dt);
		if (this->colorTransformOffset > this->targetColorTransformOffset)
		{
			this->colorTransformOffset = this->targetColorTransformOffset;
		}
	}
	this->colorTransform->redMultiplier = 1 + (this->colorTransformOffset * (MAX_COLOR_TRANSFORM->redMultiplier - 1));
	this->colorTransform->greenMultiplier = 1 + (this->colorTransformOffset * (MAX_COLOR_TRANSFORM->greenMultiplier - 1));
	this->colorTransform->blueMultiplier = 1 + (this->colorTransformOffset * (MAX_COLOR_TRANSFORM->blueMultiplier - 1));
	this->colorTransform->redOffset = this->colorTransformOffset * MAX_COLOR_TRANSFORM->redOffset;
	this->colorTransform->greenOffset = this->colorTransformOffset * MAX_COLOR_TRANSFORM->greenOffset;
	this->colorTransform->blueOffset = this->colorTransformOffset * MAX_COLOR_TRANSFORM->blueOffset;
}

void TankSkin::setAlpha(double value)
{
	this->turretMesh->alpha = value;
	this->hullMesh->alpha = value;
	if (this->shadow != nullptr)
	{
		this->shadow->alpha = value * 0.6;
	}
}
