#include "allbeamproperties.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "BeamProperties.h"
#include "resource/resourceutil.h"

//private static const conBeamWidth:ConsoleVarFloat = new ConsoleVarFloat("beam_width", 100, 0, 1000);
//private static const conUnitLength:ConsoleVarFloat = new ConsoleVarFloat("beam_ulength", 500, 0, 10000);
//private static const conAnimationSpeed:ConsoleVarFloat = new ConsoleVarFloat("beam_anim_speed", -0.6, -1000, 1000);
//private static const conURange:ConsoleVarFloat = new ConsoleVarFloat("beam_urange", 0.6, 0.1, 1);
//private static const conAlpha:ConsoleVarFloat = new ConsoleVarFloat("beam_alpha", 1, 0, 1);
//private static const blueRay:Class = AllBeamProperties_blueRay;
//private static const blueRayTip:Class = AllBeamProperties_blueRayTip;
//private static const redRay:Class = AllBeamProperties_redRay;
//private static const redRayTip:Class = AllBeamProperties_redRayTip;

static const double conBeamWidth = 100;
static const double conUnitLength = 500;
static const double conAnimationSpeed = -0.6;
static const double conURange = 0.6;
static const double conAlpha = 1;



static BeamProperties* createBeamProperties(std::shared_ptr<BitmapData> &param2,
											std::shared_ptr<BitmapData> &param3,
											double beamWidth,
											double unitLength,
											double animationSpeed,
											double uRange,
											double alpha)
{
	std::shared_ptr<TextureMaterial> beamMaterial = new_TextureMaterial();
	beamMaterial->init();
	beamMaterial->setTexture(param2);
	beamMaterial->repeat = true;

	std::shared_ptr<TextureMaterial> beamTipMaterial = new_TextureMaterial();
	beamTipMaterial->init();
	beamTipMaterial->setTexture(param3);

	return new BeamProperties(beamMaterial, beamTipMaterial, beamWidth, unitLength, animationSpeed, uRange, alpha);
}


static BeamProperties* createProperties(BeamProperties *beamProperties)
{
	return new BeamProperties(beamProperties->beamMaterial,
							  beamProperties->beamTipMaterial,
							  conBeamWidth,
							  conUnitLength,
							  conAnimationSpeed,
							  conURange,
							  conAlpha);
}



AllBeamProperties::AllBeamProperties()
{
	std::shared_ptr<BitmapData> blueRay = ResourceUtil::getResourceImageLocal("dom/AllBeamProperties_blueRay.png");
	std::shared_ptr<BitmapData> blueRayTip = ResourceUtil::getResourceImageLocal("dom/AllBeamProperties_blueRayTip.png");

	std::shared_ptr<BitmapData> redRay = ResourceUtil::getResourceImageLocal("dom/AllBeamProperties_redRay.png");
	std::shared_ptr<BitmapData> redRayTip = ResourceUtil::getResourceImageLocal("dom/AllBeamProperties_redRayTip.png");

	this->blueBeamProperties = createBeamProperties(blueRay, blueRayTip, 50, 100, 1, 1, 1);
	this->redBeamProperties = createBeamProperties(redRay, redRayTip, 50, 100, 1, 1, 1);
}

BeamProperties* AllBeamProperties::getBlueBeamProperties()
{
	return createProperties(this->blueBeamProperties);
}

BeamProperties* AllBeamProperties::getRedBeamProperties()
{
	return createProperties(this->redBeamProperties);
}

BeamProperties* AllBeamProperties::getBeamProperties(BattleTeamType team)
{
	return (team == BattleTeamType::BLUE) ? this->getBlueBeamProperties() : this->getRedBeamProperties();
}
