#include "tank3d.h"
#include "alternativa/engine3d/core/shadow.h"
#include "flash/display/bitmapdata.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "tank3dpart.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/engine3d/core/MipMapping.h"
#include "alternativa/tanks/vehicles/tanks/trackskin.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/vertex.h"
#include "alternativa/tanks/materials/animatedpaintmaterial.h"
#include "resource/images/MultiframeResourceData.h"
#include "file.h"
#include "_global.h"
#include <QDebug>


const Object3D::T *Tank3D::TYPE = new Object3D::T("Tank3D");

static std::shared_ptr<BitmapData> defaultColormap;


Tank3D::Tank3D(const TT &t, Tank3DPart *hull, Tank3DPart *turret, ImageResource *colormap) :
	ConflictContainer(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	this->resolveByAABB = true;
	this->hull = nullptr;
	this->turret = nullptr;
	this->colormap = nullptr;
	this->leftTrackSkin = nullptr;
	this->rightTrackSkin = nullptr;

	this->setHull(hull);
	this->setTurret(turret);
	this->setColorMap(colormap);

#ifdef GRAPHICS
	this->shadow = new Shadow(128, 6, 100, 5000, 10000, 516, 1);
	this->shadow->offset = 100;
	this->shadow->backFadeRange = 100;
#endif
}

Tank3D::~Tank3D()
{
#ifdef GRAPHICS
	delete this->shadow;
#endif
}

#ifdef GRAPHICS
static std::shared_ptr<BitmapData> getDefaultColormap()
{
	if (defaultColormap == nullptr)
		defaultColormap = std::make_shared<BitmapData>(1, 1, false, 0x00FF00);

	return defaultColormap;
}
#endif

void Tank3D::setColorMap(ImageResource *colormap)
{
#ifdef GRAPHICS
	this->colormap = colormap != nullptr ? colormap->bitmapData->data : getDefaultColormap();

	if (this->hull != nullptr)
	{
		this->hull->animatedPaint = ((!(colormap == nullptr)) ? colormap->animatedMaterial : false);
	}

	if (this->turret != nullptr)
	{
		this->turret->animatedPaint = ((!(colormap == nullptr)) ? colormap->animatedMaterial : false);
	}

	this->animatedColormap = colormap != nullptr ? colormap->animatedMaterial : false;
	this->multiframeData = colormap != nullptr ? colormap->multiframeData : nullptr;

	this->updatePartTexture(this->hull);
	this->updatePartTexture(this->turret);
#else
	(void)colormap;
#endif
}

void Tank3D::setHull(Tank3DPart *value)
{
	if (this->hull != nullptr)
		removeChild(this->hull->mesh.get());

	if (value == nullptr)
		return;

	this->hull = value;
	addChild(this->hull->mesh.get());
	this->hull->mesh->position.reset(0, 0, 0);

	this->updatePartTexture(this->hull);
	this->updateMountPoint();
#ifdef GRAPHICS
	this->shadow->addCaster(this->hull->mesh.get());
#endif
}

void Tank3D::setTurret(Tank3DPart *value)
{
	if (this->turret != nullptr)
		removeChild(this->turret->mesh.get());

	if (value == nullptr)
		return;

	this->turret = value;
	addChild(this->turret->mesh.get());

	this->updatePartTexture(this->turret);
	this->updateMountPoint();
#ifdef GRAPHICS
	this->shadow->addCaster(this->turret->mesh.get());
#endif
}

void Tank3D::updatePartTexture(Tank3DPart *part)
{
#ifdef GRAPHICS
	if (part == nullptr || this->colormap == nullptr)
		return;

	std::shared_ptr<BitmapData> texture = std::make_shared<BitmapData>(part->lightmap->width(), part->lightmap->height(), false, 0);

	texture->beginBitmapFill(this->colormap.get());
	texture->draw(part->lightmap.get(), nullptr, nullptr, BlendMode::HARDLIGHT);
	texture->draw(part->details.get());


	std::shared_ptr<TextureMaterial> material;

	if (this->animatedColormap)
	{
		material = new_AnimatedPaintMaterial(this->colormap,
											 part->lightmap,
											 part->details,
											 this->colormap->width() / this->multiframeData->widthFrame,
											 this->colormap->height() / this->multiframeData->heigthFrame,
											 this->multiframeData->fps,
											 this->multiframeData->numFrames,
											 MipMapping::OBJECT_DISTANCE);
	}
	else
	{
		material = new_TextureMaterial();
		material->init(false, true, MipMapping::PER_PIXEL, part->mesh->calculateResolution(texture->width(), texture->height()));
		material->setTexture(texture);
	}

	this->createTrackSkins(part->mesh.get(), material);
	part->mesh->setMaterialToAllFaces(material.get());
#else
	(void)part;
#endif
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

void Tank3D::createTrackSkins(Mesh *mesh, std::shared_ptr<TextureMaterial> material)
{
#ifdef GRAPHICS
	this->leftTrackSkin = new TrackSkin();
	this->rightTrackSkin = new TrackSkin();

	QVector<std::shared_ptr<Face>> faces = getFaces(mesh);

	for (std::shared_ptr<Face> face : faces)
	{
		if (face->material->getName() == "track") //face->id == "track" ||
		{
			this->addFaceToTrackSkin(face);
		}
	}

	this->leftTrackSkin->init();
	this->rightTrackSkin->init();
	this->leftTrackSkin->setMaterial(TextureMaterial_clone(material));
	this->rightTrackSkin->setMaterial(TextureMaterial_clone(material));
#else
	(void)mesh;
	(void)material;
#endif
}

void Tank3D::addFaceToTrackSkin(std::shared_ptr<Face> face)
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

void Tank3D::updateMountPoint()
{
	if (this->hull == nullptr || this->turret == nullptr)
		return;

	this->turret->mesh->position.x = this->hull->turretMountPoint->x;
	this->turret->mesh->position.y = this->hull->turretMountPoint->y;
	this->turret->mesh->position.z = this->hull->turretMountPoint->z;
}



std::shared_ptr<Tank3D> new_Tank3D(Tank3DPart *hull, Tank3DPart *turret, ImageResource *colormap)
{
	return std::make_shared<Tank3D>(Object3D::TT(Tank3D::TYPE), hull, turret, colormap);
}
