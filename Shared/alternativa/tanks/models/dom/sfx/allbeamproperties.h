#ifndef ALLBEAMPROPERTIES_H
#define ALLBEAMPROPERTIES_H

class BeamProperties;
enum class BattleTeamType;


class AllBeamProperties
{
	BeamProperties *blueBeamProperties;
	BeamProperties *redBeamProperties;
public:

	AllBeamProperties();
	BeamProperties* getBlueBeamProperties();
	BeamProperties* getRedBeamProperties();
	BeamProperties* getBeamProperties(BattleTeamType team);
};

#endif // ALLBEAMPROPERTIES_H
