#include "playerinfo.h"

static const uint32_t normalGlowColor = 0x12FF00;
static const uint32_t minusGlowColor = 0xFF4000;

PlayerInfo::PlayerInfo()
{
	//indicators = new Indicators();
	//addChild(this->indicators);
	//this->indicators.removeChild(this->indicators.changeButton);
	//addEventListener(Event.ADDED_TO_STAGE, this->configUI);
}

QString PlayerInfo::getPlayerName()
{
	return (this->_playerName);
}

void PlayerInfo::setPlayerName(const QString &name)
{
	this->_playerName = name;
	this->updateInfo();
}

int PlayerInfo::getRang()
{
	return this->_rang;
}

void PlayerInfo::setRang(int value)
{
	this->_rang = value;
	this->updateInfo();
}

void PlayerInfo::updateScore(int scoreValue, int scoreRemainValue)
{
	(void)scoreValue;
	(void)scoreRemainValue;
	/*
	if (((!(scoreValue == this->_score)) && (!(this->_score == 0))))
	{
		this->flashLabel(this->indicators.playerInfo, ((scoreValue > this->_score) ? this->normalGlowColor : this->minusGlowColor));
	}
	this->_score = scoreValue;
	this->_scoreRemain = scoreRemainValue;
	this->updateInfo();
	*/
}

int PlayerInfo::getProgress()
{
	return (this->_progress);
}

void PlayerInfo::setProgress(int value)
{
	if (this->_progress == 0)
	{
		this->_progress = value;
	}
	else
	{
		this->_newProgress = value;
		this->_progress = value;
		//this->indicators.newprogress = value;
	}
	this->updateInfo();
}

double PlayerInfo::getRating()
{
	return (this->_rating);
}

void PlayerInfo::setRating(double rating)
{
	if (((!(int(rating) == this->_rating)) && (!(this->_rating == 0))))
	{
		//this->flashLabel(this->indicators.kdRatio, ((int(rating) > this->_rating) ? this->normalGlowColor : this->minusGlowColor));
	}
	this->_rating = int(rating);
	this->updateInfo();
}

int PlayerInfo::getRatingChange()
{
	return (this->_rating);
}

void PlayerInfo::setRatingChange(int change)
{
	this->_ratingChange = change;
	this->updateInfo();
}

int PlayerInfo::getPosition()
{
	return (this->_position);
}

void PlayerInfo::setPosition(int pos)
{
	if (((!(pos == this->_position)) && (!(this->_position == 0))))
	{
		//this->flashLabel(this->indicators.scoreLabel, ((pos > this->_position) ? this->minusGlowColor : this->normalGlowColor));
	}
	this->_position = pos;
	this->updateInfo();
}

int PlayerInfo::getShield()
{
	return (this->_shield);
}

void PlayerInfo::setShield(int value)
{
	this->_shield = value;
	this->updateInfo();
}

int PlayerInfo::getWeapon()
{
	return (this->_weapon);
}

void PlayerInfo::setWeapon(int value)
{
	this->_weapon = value;
	this->updateInfo();
}

int PlayerInfo::getEngine()
{
	return (this->_engine);
}

void PlayerInfo::setEngine(int value)
{
	this->_engine = value;
	this->updateInfo();
}

int PlayerInfo::getMoney()
{
	return this->_money;
}

void PlayerInfo::setMoney(int value)
{
	this->_money = value;
}

int PlayerInfo::getCrystals()
{
	return (this->_engine);
}

void PlayerInfo::setCrystals(int value)
{
	if (((!(value == this->_crystals)) && (!(this->_crystals == 0))))
	{
		//this->flashLabel(this->indicators.crystalInfo, ((value > this->_crystals) ? this->normalGlowColor : this->minusGlowColor));
	}
	this->_crystals = value;
	this->updateInfo();
}

void PlayerInfo::configUI()
{
	//this->indicators.x = 59;
	//removeEventListener(Event.ADDED_TO_STAGE, this->configUI);
}

void PlayerInfo::updateInfo()
{
	/*
	var localeService:ILocaleService = (Main.osgi.getService(ILocaleService) as ILocaleService);
	this->indicators.playerInfo.text = ((((((String(this->_score) + " / ") + String(this->_scoreRemain)) + "   ") + Rank.name(this->_rang)) + " ") + this->_playerName);
	this->indicators.progress = this->_progress;
	this->indicators.kdRatio.text = String(int(this->_rating));
	this->indicators.kd_icon.gotoAndStop((this->_ratingChange + 2));
	this->indicators.scoreLabel.text = (localeService.getText(TextConst.MAIN_PANEL_RATING_LABEL) + String(this->_position));
	switch (this->_ratingChange)
	{
		case -1:
			this->indicators.kdRatio.color = 0xFF1500;
			break;
		case 0:
			this->indicators.kdRatio.color = 0xB2B2B2;
			break;
		case 1:
			this->indicators.kdRatio.color = 0x12FF00;
			break;
	};
	this->indicators.crystalInfo.text = Money.numToString(this->_crystals, false);
	this->width = this->_width;
	*/
}

void PlayerInfo::flashLabel(Label *target, uint32_t color)
{
	(void)target;
	(void)color;
	//this->glowAlpha[target.name] = 1;
	//this->glowColor[target.name] = color;
	//target.addEventListener(Event.ENTER_FRAME, this->glowFrame);
}

void PlayerInfo::glowFrame(Event *e)
{
	(void)e;
	/*
	var trgt:Label = (e.target as Label);
	var filter:GlowFilter = new GlowFilter(this->glowColor[trgt.name], this->glowAlpha[trgt.name], 4, 4, 3, 1, false);
	trgt.filters = [filter];
	this->glowAlpha[trgt.name] = (this->glowAlpha[trgt.name] - this->glowDelta);
	if (this->glowAlpha[trgt.name] < 0)
	{
		trgt.filters = [];
		trgt.removeEventListener(Event.ENTER_FRAME, this->glowFrame);
	}
	*/
}

void PlayerInfo::setWidth(double value)
{
	(void)value;
	//this->_width = int(value);
	//this->indicators.width = value;
}
