#ifndef TRACKMATERIAL_H
#define TRACKMATERIAL_H

#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/engine3d/materials/uvmatrixprovider.h"


class TrackMaterial : public TextureMaterial
{

public:
	UVMatrixProvider *uvMatrixProvider;

	static const T *TYPE;

	TrackMaterial(const T *type, std::shared_ptr<BitmapData> &bitmapData);
	~TrackMaterial();

	void update();
	bool transparent() override;
	void drawOpaque(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6) override;
	void drawTransparent(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6, bool param7 = false) override;

protected:
	ProgramResource* getProgram(bool param1, bool param2, bool param3, bool param4, bool param5, bool param6, bool param7, bool param8, bool param9, bool param10, bool param11, bool param12, bool param13, bool param14, bool param15, bool param16, bool param17, bool param18, bool param19, bool param20, bool param21, bool param22) override;
};


std::shared_ptr<TrackMaterial> new_TrackMaterial(std::shared_ptr<BitmapData> &bitmapData);


#endif // TRACKMATERIAL_H
