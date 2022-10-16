#ifndef BONUSCOMMONMODEL_H
#define BONUSCOMMONMODEL_H

#include <memory>

class ClientObject;
class QString;
class IResourceService;
class IBonus;
class Mesh;
class ClientObjectBonus;


class BonusCommonModel
{
public:
	static void initObject(ClientObjectBonus *clientObject, const QString &boxResourceId, const QString &cordResourceId, int disappearingTime, const QString &parachuteInnerResourceId, const QString &parachuteResourceId);
	static IBonus* getBonus(ClientObjectBonus *clientObject, const QString &bonusId, int livingTime, bool isFalling);

private:
	 static std::shared_ptr<Mesh> getMeshFromResource(IResourceService *resourceService, const QString &resourceId, bool lacklisted = false);
};

#endif // BONUSCOMMONMODEL_H
