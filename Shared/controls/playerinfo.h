#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <QString>

class Indicators;
class Label;
class Event;


class PlayerInfo
{
	QString _playerName;
	int _rang;
	int _score = 0;
	int _scoreRemain = 0;
	int _progress = 0;
	int _newProgress;
	double _deltaProgress;
	double _rating = 0;
	int _ratingChange = 0;
	int _position;
	int _shield = 5000;
	int _weapon = 7500;
	int _engine = 6000;
	int _money = 0;
	int _crystals = 0;
	int _width;

	//glowAlpha:Array = new Array();
	//glowColor:Array = new Array();
	//glowDelta:Number = 0.02;


public:
	Indicators *indicators;

	PlayerInfo();

	QString getPlayerName();
	void setPlayerName(const QString &name);

	int getRang();
	void setRang(int value);

	void updateScore(int scoreValue, int scoreRemainValue);

	int getProgress();
	void setProgress(int value);

	double getRating();
	void setRating(double rating);

	int getRatingChange();
	void setRatingChange(int change);

	int getPosition();
	void setPosition(int pos);

	int getShield();
	void setShield(int value);

	int getWeapon();
	void setWeapon(int value);

	int getEngine();
	void setEngine(int value);

	int getMoney();
	void setMoney(int value);

	int getCrystals();
	void setCrystals(int value);

private:
	void configUI();
	void updateInfo();
	void flashLabel(Label *target, uint32_t color=0xFF0000);
	void glowFrame(Event *e);
	void setWidth(double value);
};

#endif // PLAYERINFO_H
