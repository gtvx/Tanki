#include "newyeargamemode.h"
#include "../battleview3d.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/engine3d/lights/directionallight.h"
#include "alternativa/engine3d/core/shadowmap.h"
#include "alternativa/tanks/model/panel/IBattleSettings.h"
#include "alternativa/math/matrix3.h"


void NewYearGameMode::applyChanges(BattleView3D *viewport)
{
	GameCamera *camera = viewport->camera;
	this->camera = camera;
	camera->directionalLightStrength = 1;
	camera->ambientColor = 794160;
	camera->deferredLighting = true;
	light = new_DirectionalLight(0x171717);
	light->useShadowMap = true;
	double x = -1.1;
	double z = -3.8;
	Matrix3 matrix;
	matrix.setRotationMatrix(x, 0, z);
	Vector3 toPos(0, 1, 0);
	toPos.transformBy3(&matrix);
	light->lookAt(toPos.x, toPos.y, toPos.z);
	light->intensity = 1;
	camera->directionalLight = light.get();
	camera->fogColor = 0x6A6A6A;
	camera->fogNear = 100;
	camera->fogFar = 5000;
	camera->fogStrength = 1;
	camera->fogAlpha = 0.9;
	camera->shadowMap = new ShadowMap(0x0800, 5000, 10000, 0, 0);
	camera->shadowMapStrength = 1;
	camera->shadowMap->bias = 0.5;
	camera->shadowMap->biasMultiplier = 30;
	camera->shadowMap->additionalSpace = 10000;
	camera->shadowMap->alphaThreshold = 0.1;
	camera->useShadowMap = true;
}

void NewYearGameMode::applyChangesBeforeSettings(IBattleSettings *settings)
{
	if (settings->fog() && this->camera->fogStrength != 1)
	{
		this->camera->fogColor = 0xFFFFFF;
		this->camera->fogNear = 5000;
		this->camera->fogFar = 10000;
		this->camera->fogStrength = 1;
		this->camera->fogAlpha = 0.35;
	}
	else
	{
		if (!settings->fog())
		{
			this->camera->fogStrength = 0;
		}
	}
	if (settings->shadows() && !this->camera->useShadowMap)
	{
		this->camera->useShadowMap = true;
		if (this->camera->directionalLight != nullptr)
		{
			this->camera->directionalLight->useShadowMap = true;
		}
		this->camera->shadowMapStrength = 1;
	}
	else
	{
		if (!settings->shadows())
		{
			this->camera->useShadowMap = false;
			if (this->camera->directionalLight != nullptr)
			{
				this->camera->directionalLight->useShadowMap = false;
			}
			this->camera->shadowMapStrength = 0;
		}
	}

	if (settings->defferedLighting() && this->camera->directionalLightStrength != 1)
	{
		this->camera->directionalLight->intensity = 1;
		this->camera->directionalLightStrength = 1;
		this->camera->deferredLighting = true;
		this->camera->deferredLightingStrength = 1;
	}
	else
	{
		if (!settings->defferedLighting())
		{
			this->camera->directionalLight->intensity = 0;
			this->camera->directionalLightStrength = 0;
			this->camera->deferredLighting = false;
			this->camera->deferredLightingStrength = 0;
		}
	}
}

BitmapData* NewYearGameMode::applyColorchangesToSkybox(BitmapData *skybox)
{
	(void)skybox;
	//var btm:BitmapData = new BitmapData(1, 1, false, (1382169 + 7559484));
	//skybox.colorTransform(skybox.rect, new Bitmap(btm).transform.colorTransform);
	//return skybox;
	return nullptr;
}
