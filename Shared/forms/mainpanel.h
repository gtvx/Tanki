#ifndef MAINPANEL_H
#define MAINPANEL_H

class RangIconNormal;
class PlayerInfo;
class ButtonBar;
class Event;
class MainPanelRank;
class Stage;


class MainPanel
{
	int _rang;
	bool _isTester;

public:

	RangIconNormal* rangIcon;
	PlayerInfo *playerInfo;
	ButtonBar *buttonBar;

	MainPanelRank *rank;

	MainPanel(bool isTester = false);
	void setRang(int value);
	int getRang();

private:
	void configUI(Event *e);
	void listClick();
	void onResize();

public:
	void hide();
	bool getIsTester();
	void setIsTester(bool value);
	void draw(Stage *stage);
};

#endif // MAINPANEL_H
