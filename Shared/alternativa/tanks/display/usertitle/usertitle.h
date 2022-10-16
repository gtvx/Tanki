#ifndef USERTITLE_H
#define USERTITLE_H

#include "flash/geom/geomrectangle.h"
#include <QString>
#include <QFont>
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include <memory>


class UserTitleProgressBar;
class ProgressBarSkin;
class TextureMaterial;
class BitmapData;
class Sprite3D;
class Material;
class EffectIndicator;
class Vector3;
class Scene3DContainer;


class UserTitle
{
	int configFlags;
	int dirtyFlags;

	GeomRectangle textureRect;
	QFont font;
	int label_textWidth;
	//Label label;
	UserTitleProgressBar *healthBar;
	UserTitleProgressBar *weaponBar;
	//Vector.<EffectIndicator> effectIndicators;
	EffectIndicator *effectIndicators[4];
	int numVisibleIndicators;
	int effectIndicatorsY;
	int rankId;
	QString labelText;
	int health;
	int weaponStatus;
	bool indicatorsNumberChanged;
	BattleTeamType teamType;
	ProgressBarSkin const *skin;
	bool hidden;
	int time;
	std::shared_ptr<TextureMaterial> material;
	std::shared_ptr<BitmapData> texture;
	bool local;
	int _indexFrame;

public:

	static const int BIT_LABEL;
	static const int BIT_HEALTH;
	static const int BIT_WEAPON;
	static const int BIT_EFFECTS;


	std::shared_ptr<Sprite3D> sprite;

	UserTitle(bool isLocal);

	BitmapData* getTexture();
	void hide();
	void show();
	bool isHidden();
	void setConfiguration(int configFlags);
	void setTeamType(BattleTeamType teamType);
	void setRank(int rankId);
	void setLabelText(const QString &labelText);
	void setHealth(int health);
	void setWeaponStatus(int weaponStatus);
	void showIndicator(int effectId, int duration);
	void hideIndicator(int effectId);
	void hideIndicators();
	void doHideIndicator(EffectIndicator *indicator);
	void update(const Vector3 *pos);

	int indexFrame() { return this->_indexFrame; }

private:
	bool isDirtyAndHasOption(int bit);
	void updateEffectIndicators(int now, int delta);
	void changeVisibleIndicatorsNumber(int increment);
	void updateConfiguration();
	void setupTexture();
	void setupComponents();

public:
	void addToContainer(Scene3DContainer *container);
	void removeFromContainer();
	void setPosition(const Vector3 *pos);

private:
	void markDirty(int dirtyBit);

public:
	bool hasOption(int optionBit);	
	void updateTexture();

private:
	void createLabelComponents();
	void updateLabel();
	void createEffectsIndicators();
	EffectIndicator* getEffectIndicatorById(int effectId);
	void updateVisibility(int delta);
};

#endif // USERTITLE_H
