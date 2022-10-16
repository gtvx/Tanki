#ifndef PAINTMATERIAL_H
#define PAINTMATERIAL_H

#include "alternativa/engine3d/materials/texturematerial.h"

class PaintMaterial : public TextureMaterial
{
protected:

	double fragConst[8];

	std::shared_ptr<BitmapData> spriteSheetBitmap;
	std::shared_ptr<BitmapData> lightMapBitmap;

	std::shared_ptr<BitmapTextureResource> _spriteSheetResource;
	std::shared_ptr<BitmapTextureResource> _lightMapResource;

	BitmapTextureResource *spriteSheetResource;
	BitmapTextureResource *lightMapResource;


public:

	static const T *TYPE;

	PaintMaterial(const T *type,
				  std::shared_ptr<BitmapData> spriteSheetBitmap,
				  std::shared_ptr<BitmapData> lightMapBitmap,
				  std::shared_ptr<BitmapData> param3,
				  MipMapping mipMapping = MipMapping::NONE);

	bool transparent() override;
	void drawOpaque(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6) override;
	void drawTransparent(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6, bool param7 = false) override;
	void disposeResource() override;
	void dispose() override;

protected:
	ProgramResource* getProgram(bool param1, bool param2, bool param3, bool param4, bool param5,
								bool param6, bool param7, bool param8, bool param9, bool param10,
								bool param11, bool param12, bool param13, bool param14, bool param15,
								bool param16, bool param17, bool param18, bool param19, bool param20,
								bool param21, bool param22) override;
};

std::shared_ptr<PaintMaterial> new_PaintMaterial(std::shared_ptr<BitmapData> spriteSheetBitmap,
												 std::shared_ptr<BitmapData> lightMapBitmap,
												 std::shared_ptr<BitmapData> param3,
												 MipMapping mipMapping = MipMapping::NONE);

#endif // PAINTMATERIAL_H
