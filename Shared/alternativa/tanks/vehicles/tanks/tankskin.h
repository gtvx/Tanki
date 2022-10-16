#ifndef TANKSKIN_H
#define TANKSKIN_H

#include <memory>

class QString;
class TankSkinHull;
class TankSkinTurret;
class ImageResource;
class ITextureMaterialRegistry;
class BitmapData;
class Vector3;
class Mesh;
class BattlefieldModel;
class Quaternion;
class TrackSkin;
class Face;
class SkinStateMaterials;
class ITextureMaterialRegistry;
class ColorTransform;
class Scene3DContainer;
class TankSkinPrivate;
class Shadow;


class TankSkin
{
	int skinState;
	BattlefieldModel *bfModel;
	std::shared_ptr<Mesh> initHull(TankSkinHull *hullDescriptor);
	std::shared_ptr<Mesh> initTurret(TankSkinTurret *turretDescriptor);
	std::shared_ptr<Mesh> createMesh(std::shared_ptr<Mesh> &referenceMesh);

	TrackSkin *leftTrackSkin;
	TrackSkin *rightTrackSkin;

	std::shared_ptr<ColorTransform> colorTransform;

	ImageResource *coloring;

	TankSkinPrivate *d;

	std::shared_ptr<SkinStateMaterials> normalMaterials;
	std::shared_ptr<SkinStateMaterials> deadMaterials;

	ITextureMaterialRegistry *textureRegistry;
	ITextureMaterialRegistry *registry;

	Scene3DContainer *container;
	Shadow *shadow;

	double colorTransformOffset;

public:
	double turretDirection;
	TankSkinHull *hullDescriptor;
	TankSkinTurret *turretDescriptor;
	std::shared_ptr<Mesh> _turretMesh;
	std::shared_ptr<Mesh> _hullMesh;

	Mesh *turretMesh;
	Mesh *hullMesh;

	double targetColorTransformOffset;

	TankSkin(TankSkinHull *hull,
			 TankSkinTurret *turret,
			 ImageResource *normalColoring,
			 const QString &lightmapHullId,
			 const QString &detailsHullId,
			 const QString &lightmapTurretid,
			 const QString &detailsTurretId,
			 ITextureMaterialRegistry *textureMaterialRegistry);
	~TankSkin();

	static void _clear();

	void updateHullTransform(const Vector3 *position, const Vector3 *rotation);
	void setNormalState();

	void setDeadState();
	void updateTransform(Vector3 *pos, Quaternion *orientation);
	void dispose();
	void resetColorTransform();
	void addToContainer(Scene3DContainer *container);
	void removeFromContainer();
	void updateTracks(double left, double right);	
	void updateColorTransform(double dt);
	void setAlpha(double value);

private:
	void updateTurretTransform();
	void createTrackSkins();
	void addFaceToTrackSkin(std::shared_ptr<Face> face);
};

#endif // TANKSKIN_H
