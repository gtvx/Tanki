#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{
public:
	QString login;
	QString password;

	bool antialiasing;

	bool bot;
	bool bot_shoot;
	bool bot_carry_flag;
	bool bot_capture_points;
	bool bot_auto_kill;
	bool bot_random_path;
	bool bot_target_tank;
	bool bot_body_turn;
};


#endif // SETTINGS_H
