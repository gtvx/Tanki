#ifndef INDICATORS_H
#define INDICATORS_H

class TimerEvent;


class Indicators
{
	//public var CR:MovieClip;
	//public var C1:MovieClip;
	//public var kdicon:MovieClip;
	//public var L:MovieClip;
	//public var N:MovieClip;
	//public var _changeButton:MainPanelChangeButton;
	//public var R:MovieClip;
	//public var playerInfo:Label = new Label();
	//public var scoreLabel:Label = new Label();
	//public var kdRatio:Label = new Label();
	//public var crystalInfo:Label = new Label();
	//public var changeButton:BaseButton;
	//public var kd_icon:MovieClip;
	int _width;
	//private var progressBar:Sprite = new Sprite();
	//private var b_left:MovieClip;
	//private var b_name:MovieClip;
	//private var b_conn1:MovieClip;
	//private var b_ind:MovieClip;
	//private var b_conn2:MovieClip;
	//private var b_money:MovieClip;
	//private var b_conn3:MovieClip;
	//private var b_crystal:MovieClip;
	//private var b_right:MovieClip;
	//private var timer:Timer = new Timer(50);
	//private var redZone:Sprite = null;
	int _progress;
	int _nprogress;
	int _oprogress;
public:

	Indicators();

	int getProgress();
	void setProgress(int);
	void setNewProgress(int);

private:
	void hideRedZone(TimerEvent*);
	void setWidth(double);
};

#endif // INDICATORS_H
