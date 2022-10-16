#ifndef ANIMATEDPAINTMATERIAL_H
#define ANIMATEDPAINTMATERIAL_H

#include "paintmaterial.h"

class AnimatedPaintMaterial : public PaintMaterial
{
	int lastFrame;
	int fps;
	int numFrames;
	int numFramesX;
	int numFramesY;
	int time;
	double currentFrame;
	double scaleX;
	double scaleY;
	double frameWidth;
	double frameHeight;


public:

	static const T *TYPE;

	AnimatedPaintMaterial(const T *type,
						  std::shared_ptr<BitmapData> &param1,
						  std::shared_ptr<BitmapData> &param2,
						  std::shared_ptr<BitmapData> &param3,
						  int numFramesX,
						  int numFramesY,
						  int fps,
						  int numFrames,
						  MipMapping mipMapping = MipMapping::NONE);


	void update();
	void drawOpaque(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6) override;
	void drawTransparent(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6, bool param7 = false) override;

protected:
	ProgramResource* getProgram(bool param1, bool param2, bool param3, bool param4, bool param5, bool param6,
								bool param7, bool param8, bool param9, bool param10, bool param11, bool param12,
								bool param13, bool param14, bool param15, bool param16, bool param17, bool param18,
								bool param19, bool param20, bool param21, bool param22) override;

};

std::shared_ptr<AnimatedPaintMaterial> new_AnimatedPaintMaterial(std::shared_ptr<BitmapData> &param1,
																 std::shared_ptr<BitmapData> &param2,
																 std::shared_ptr<BitmapData> &param3,
																 int numFramesX,
																 int numFramesY,
																 int fps,
																 int numFrames,
																 MipMapping mipMapping = MipMapping::NONE);


#endif // ANIMATEDPAINTMATERIAL_H
