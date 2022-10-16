#ifndef TANKCRITICALHITMODEL_H
#define TANKCRITICALHITMODEL_H

//extends TankCriticalHitModelBase implements ITankCriticalHitModelBase, IObjectLoadListener, ITankCriticalHitModel
class QString;
class ClientObject;
class TankData;
class ClientObjectHull;


class TankCriticalHitModel
{
	/*
	private var objectPoolService:IObjectPoolService = IObjectPoolService(Main.osgi.getService(IObjectPoolService));
	private var materialRegistry:IMaterialRegistry = IMaterialRegistry(Main.osgi.getService(IMaterialRegistry));
	private var explosionSize:ConsoleVarFloat = new ConsoleVarFloat("tankexpl_size", EXPLOSION_SIZE, 1, 500);
	private var smokeSize:ConsoleVarFloat;
	private var scaleModifier:ConsoleVarFloat = new ConsoleVarFloat("tankexpl_scale", 1, 0, 10);
	private var shockWaveScaleSpeed:ConsoleVarFloat;
	private var battlefieldModel:IBattleField;
	private var rayHit:RayIntersection = new RayIntersection();
	private var position:Vector3 = new Vector3();
	private var eulerAngles:Vector3 = new Vector3();
	private var velocity:Vector3 = new Vector3();
	private var matrix:Matrix3 = new Matrix3();
	*/
public:
	TankCriticalHitModel();

	static bool initObject(ClientObjectHull *clientObject,
						   const QString &explosionTextureId,
						   const QString &shockWaveTextureId,
						   const QString &smokeTextureId);

	void objectLoaded(ClientObjectHull *object);
	void objectUnloaded(ClientObjectHull *object);
	static void createExplosionEffects(ClientObjectHull *clientObject, TankData *tankData);
};

#endif // TANKCRITICALHITMODEL_H
