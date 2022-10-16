#ifndef BOTSETTINGS_H
#define BOTSETTINGS_H


class BotSettings
{
public:
	BotSettings();

	bool shoot;
	bool carry_flag;
	bool capture_points;
	bool auto_kill;
	bool random_path;
	bool draw_points;
	bool draw_path;
	bool target_tank;
	bool body_turn;
};

#endif // BOTSETTINGS_H
