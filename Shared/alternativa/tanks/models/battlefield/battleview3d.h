#ifndef BATTLEVIEW3D_H
#define BATTLEVIEW3D_H

class GameCamera;
class ICollisionDetector;
class BattlefieldModel;
class Scene3DContainer;
class Object3DContainerProxy;
class Object3DContainer;
class Object3D;
class DecalFactory;
class Decal;
class Vector3;
enum class RotationState;
class TextureMaterial;
class FadingDecalsRenderer;
class Stage;
class AmbientShadows;
class Shadow;


#include <memory>
#include <QVector>
#include <QQueue>


class BattleView3D
{
	//private var container:Point = new Point();
	//private const temporaryDecals:Queue = new Queue();
	//private const allDecals:Dictionary = new Dictionary();
	//private const excludedObjects:Dictionary = new Dictionary();
	//private var ambientShadows:AmbientShadows;
	//private var mainContainer:Object3DContainer;
	//private var frontContainer:Object3DContainer;
	//private var mapContainer:Object3DContainer;
	//private var mapContainerProxy:Object3DContainerProxy;
	//private var frontContainerProxy:Object3DContainerProxy;
	//private var _showAxisIndicator:Boolean;
	//private var axisIndicator:AxisIndicator;
	//private var decalFactory:DecalFactory;
	//private var light:DirectionalLight;
	//private var fadingDecalRenderer:FadingDecalsRenderer;
	//private const BG_COLOR:uint = 0x999900;

	//private var shadowsActivated:Boolean = false;


	std::shared_ptr<Object3DContainerProxy> frontContainerProxy;
	std::shared_ptr<Object3DContainerProxy> mapContainerProxy;

	std::shared_ptr<Object3DContainer> skyboxContainer;
	std::shared_ptr<Object3DContainer> rootContainer;
	std::shared_ptr<Object3DContainer> frontContainer;
	std::shared_ptr<Object3DContainer> mapContainer;
	std::shared_ptr<Object3DContainer> mainContainer;

	DecalFactory *decalFactory;

	QVector<std::shared_ptr<Decal>> allDecals;
	QQueue<std::shared_ptr<Decal>> temporaryDecals;


	FadingDecalsRenderer *fadingDecalRenderer;

	Stage *stage;

	AmbientShadows *ambientShadows;

public:
	BattleView3D(bool showAxisIndicator, ICollisionDetector *collisionDetector, BattlefieldModel *bs);
	~BattleView3D();

	//public static const MAX_SCREEN_SIZE:int = 10;

	//public var overlay:Sprite;
	std::shared_ptr<GameCamera> _camera;
	GameCamera *camera;

	//public const shaftRaycastExcludedObjects:Dictionary = new Dictionary();
	void update();

	Scene3DContainer* getFrontContainer();

	Scene3DContainer* getMapContainer();


	void setStage(Stage *stage);
	Stage* getStage() { return stage; }

	void resize(double w, double h);
	void setMapContainer(std::shared_ptr<Object3DContainer> value);

	void setSkyBox(Object3D *object);

	Object3DContainer* get_MapContainer();

	void clearContainers();

	void addDecal(const Vector3 *hitPosition, const Vector3 *muzzlePosition, double radius, TextureMaterial *material, RotationState rotationState);
	void removeDecal(Decal *decal);

	void enableSoftParticles();
	void disableSoftParticles();

	void enableAmbientShadows(bool enable);
	void addAmbientShadow(Shadow *shadow);
	void removeAmbientShadow(Shadow *shadow);

	void enableFog();
	void disableFog();
};

#endif // BATTLEVIEW3D_H
