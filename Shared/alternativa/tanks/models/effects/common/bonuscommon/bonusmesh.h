#ifndef BONUSMESH_H
#define BONUSMESH_H

#include "alternativa/engine3d/objects/mesh.h"

class BonusMesh : public Mesh
{
	QString *objectId;
	double ownAlpha;
	double alphaMultiplier;
	void updateAlpha();

public:

	static const T *TYPE;
	const T *__type_next;


	BonusMesh(const TT &t, const QString &objectId, Mesh *sourceMesh);
	virtual ~BonusMesh();

	QString getObjectId();
	double getAlpha();
	void setAlpha(double value);
	void readPosition(Vector3 *result);
	void setAlphaMultiplier(double value);
	void recycle();
};

std::shared_ptr<BonusMesh> new_BonusMesh(const QString &objectId, Mesh *sourceMesh);

#endif // BONUSMESH_H
