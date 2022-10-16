#ifndef MAINPANELRANK_H
#define MAINPANELRANK_H

class Stage;
class BitmapData;

#include <memory>


class MainPanelRank
{
	int _value;
	std::shared_ptr<BitmapData> icon;

public:
	MainPanelRank(int value);
	void setValue(int value);
	void draw(Stage *stage);
};

#endif // MAINPANELRANK_H
