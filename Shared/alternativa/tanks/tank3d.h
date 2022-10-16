#ifndef TANK3D_H
#define TANK3D_H

#include "alternativa/engine3d/containers/conflictcontainer.h"

class Tank3DPart;
class BitmapData;
class MultiframeResourceData;
class TrackSkin;
class Shadow;
class ImageResource;
class Mesh;
class Material;
class TextureMaterial;


class Tank3D : public ConflictContainer
{
public:
	Tank3DPart *hull;
	Tank3DPart *turret;
	std::shared_ptr<BitmapData> colormap;
	bool animatedColormap;
	MultiframeResourceData *multiframeData;

	TrackSkin *leftTrackSkin;
	TrackSkin *rightTrackSkin;

public:

	static const T *TYPE;
	const T *__type_next;

	Shadow *shadow;

	Tank3D(const TT &t, Tank3DPart *hull, Tank3DPart *turret, ImageResource *colormap);
	~Tank3D();

public:
	void setColorMap(ImageResource *colormap);
	void setHull(Tank3DPart *value);
	void setTurret(Tank3DPart *value);

private:
	void updatePartTexture(Tank3DPart *part);

public:
	void createTrackSkins(Mesh *param1, std::shared_ptr<TextureMaterial> material);

private:
	void addFaceToTrackSkin(std::shared_ptr<Face> face);
	void updateMountPoint();
};

std::shared_ptr<Tank3D> new_Tank3D(Tank3DPart *hull, Tank3DPart *turret, ImageResource *colormap);

#endif // TANK3D_H
