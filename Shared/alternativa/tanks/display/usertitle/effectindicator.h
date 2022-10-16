#ifndef EFFECTINDICATOR_H
#define EFFECTINDICATOR_H

class BitmapData;
class Blinker;
class UserTitle;
class TextureMaterial;


class EffectIndicator
{
	int _effectId;
	BitmapData *icon;
	int blinkingTime;
	//colorTransform:ColorTransform = new ColorTransform();
	int blinkingStartTime;
	Blinker *blinker;
	double alpha = 1;
	bool needRedraw;
	int x;
	int y;
	UserTitle *userTitle;
	int state;
	bool blinking;
public:	

	static void _clear();

	EffectIndicator(int effectId, int blinkingTime, UserTitle *userTitle, int initialBlinkInterval, int blinkIntervalDecrement);

	int effectId() { return this->_effectId; }

	bool isVisible();
	bool isHidden();
	void show(int duration);
	void hide();
	void clear(BitmapData *texture);
	void setPosition(int x, int y);
	void forceRedraw();
	void update(int now, int delta, BitmapData *texture, TextureMaterial *sprite);

private:
	void updateBlinking(int now, int delta, BitmapData *texture, TextureMaterial *sprite);
	void draw(BitmapData *texture, TextureMaterial *sprite);
};

#endif // EFFECTINDICATOR_H
