#include "spacegamemode.h"
#include "alternativa/engine3d/lights/directionallight.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/model/panel/IBattleSettings.h"
#include "alternativa/engine3d/core/shadowmap.h"
#include "../battleview3d.h"


void SpaceGameMode::applyChanges(BattleView3D *viewport)
{
	GameCamera *camera = viewport->camera;
	this->camera = camera;
	camera->directionalLightStrength = 1;
	camera->ambientColor = 989739;
	camera->deferredLighting = true;
	int dirLightColor = 2698547;
	light = new_DirectionalLight(dirLightColor);
	light->useShadowMap = true;
	light->rotation.x = -2.420796;
	light->rotation.y = -1.1;
	light->rotation.z = 7.15;
	light->intensity = 1;
	camera->directionalLight = light.get();
	camera->shadowMap = new ShadowMap(0x0800, 5000, 10000, 0, 0);
	camera->shadowMapStrength = 1;
	camera->shadowMap->bias = 0.5;
	camera->shadowMap->biasMultiplier = 30;
	camera->shadowMap->additionalSpace = 10000;
	camera->shadowMap->alphaThreshold = 0.1;
	camera->useShadowMap = true;
}

void SpaceGameMode::applyChangesBeforeSettings(IBattleSettings *settings)
{
	if (settings->fog() && this->camera->fogStrength != 1)
	{
		this->camera->fogStrength = 1;
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

	this->camera->ssao = settings->useSSAO();
}

BitmapData* SpaceGameMode::applyColorchangesToSkybox(BitmapData *skybox)
{
	(void)skybox;
	//var btm:BitmapData = new BitmapData(1, 1, false, (1382169 + 7559484));
	//skybox.colorTransform(skybox.rect, new Bitmap(btm).transform.colorTransform);
	//return (skybox);
	return nullptr;
}
