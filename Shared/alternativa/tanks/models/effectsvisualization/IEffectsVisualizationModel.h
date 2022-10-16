#ifndef IEFFECTSVISUALIZATIONMODEL_H
#define IEFFECTSVISUALIZATIONMODEL_H

#include <QVector>

class ClientBattleEffect;

class IEffectsVisualizationModel
{
public:
	virtual QVector<ClientBattleEffect*> getInitialEffects(const QString&) = 0;
};


#endif // IEFFECTSVISUALIZATIONMODEL_H
