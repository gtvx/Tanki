#include "progressbarskin.h"
#include "resource/resourceutil.h"


namespace
{
	const uint32_t COLOR_DM = 0x4BBF1D;
	const uint32_t COLOR_DM_BG = 0x255E0F;
	const uint32_t COLOR_TEAM_BLUE = 0x4797FF;
	const uint32_t COLOR_TEAM_BLUE_BG = 0x2148FF;
	const uint32_t COLOR_TEAM_RED = 0xF03416;
	const uint32_t COLOR_TEAM_RED_BG = 0xA11609;
	const uint32_t COLOR_WEAPON_BAR = 0xD8C90F;
	const uint32_t COLOR_WEAPON_BAR_BG = 0x766204;


	const uint32_t dm_1 = 0xFF4BBF1D;
	const uint32_t dm_2 = 0xFF3A9016;
	const uint32_t dm_3 = 0xFF315721;

	const uint32_t image_hpLeftDm[8] =
	{
		dm_3, dm_2,
		dm_2, dm_1,
		dm_2, dm_1,
		dm_3, dm_2,
	};

	const uint32_t image_hpRightDm[8] =
	{
		dm_2, dm_3,
		dm_1, dm_2,
		dm_1, dm_2,
		dm_2, dm_3,
	};


	const uint32_t dm_bg_1 = 0xFF255E0F;
	const uint32_t dm_bg_2 = 0xFF1C470D;
	const uint32_t dm_bg_3 = 0xFF24371D;

	const uint32_t image_hpLeftBgDm[8] =
	{
		dm_bg_3, dm_bg_2,
		dm_bg_2, dm_bg_1,
		dm_bg_2, dm_bg_1,
		dm_bg_3, dm_bg_2,
	};

	const uint32_t image_hpRightBgDm[8] =
	{
		dm_bg_2, dm_bg_3,
		dm_bg_1, dm_bg_2,
		dm_bg_1, dm_bg_2,
		dm_bg_2, dm_bg_3,
	};


	const uint32_t blue_1 = 0xFF4797FF;
	const uint32_t blue_2 = 0xFF3773C2;
	const uint32_t blue_3 = 0xFF2F4A6D;

	const uint32_t image_hpLeftBlue[8] =
	{
		blue_3, blue_2,
		blue_2, blue_1,
		blue_2, blue_1,
		blue_3, blue_2,
	};

	const uint32_t image_hpRightBlue[8] =
	{
		blue_2, blue_3,
		blue_1, blue_2,
		blue_1, blue_2,
		blue_2, blue_3,
	};


	const uint32_t blue_bg_1 = 0xFF2148FF;
	const uint32_t blue_bg_2 = 0xFF1937C2;
	const uint32_t blue_bg_3 = 0xFF23306D;

	const uint32_t image_hpLeftBgBlue[8] =
	{
		blue_bg_3, blue_bg_2,
		blue_bg_2, blue_bg_1,
		blue_bg_2, blue_bg_1,
		blue_bg_3, blue_bg_2,
	};

	const uint32_t image_hpRightBgBlue[8] =
	{
		blue_bg_2, blue_bg_3,
		blue_bg_1, blue_bg_2,
		blue_bg_1, blue_bg_2,
		blue_bg_2, blue_bg_3,
	};


	const uint32_t red_1 = 0xFFF03416;
	const uint32_t red_2 = 0xFFB62812;
	const uint32_t red_3 = 0xFF68291F;

	const uint32_t image_hpLeftRed[8] =
	{
		red_3, red_2,
		red_2, red_1,
		red_2, red_1,
		red_3, red_2,
	};

	const uint32_t image_hpRightRed[8] =
	{
		red_2, red_3,
		red_1, red_2,
		red_1, red_2,
		red_2, red_3,
	};


	const uint32_t red_bg_1 = 0xFFA11609;
	const uint32_t red_bg_2 = 0xFF7A1207;
	const uint32_t red_bg_3 = 0xFF4D1F1B;

	const uint32_t image_hpLeftBgRed[8] =
	{
		red_bg_3, red_bg_2,
		red_bg_2, red_bg_1,
		red_bg_2, red_bg_1,
		red_bg_3, red_bg_2,
	};

	const uint32_t image_hpRightBgRed[8] =
	{
		red_bg_2, red_bg_3,
		red_bg_1, red_bg_2,
		red_bg_1, red_bg_2,
		red_bg_2, red_bg_3,
	};


	const uint32_t weapon_1 = 0xFFD8C90F;
	const uint32_t weapon_2 = 0xFFA4990D;
	const uint32_t weapon_3 = 0xFF605B1D;

	const uint32_t image_weaponLeft[8] =
	{
		weapon_3, weapon_2,
		weapon_2, weapon_1,
		weapon_2, weapon_1,
		weapon_3, weapon_2,
	};

	const uint32_t image_weaponRight[8] =
	{
		weapon_2, weapon_3,
		weapon_1, weapon_2,
		weapon_1, weapon_2,
		weapon_2, weapon_3,
	};


	const uint32_t weapon_bg_1 = 0xFF766204;
	const uint32_t weapon_bg_2 = 0xFF594A04;
	const uint32_t weapon_bg_3 = 0xFF3F3819;

	const uint32_t image_weaponBgLeft[8] =
	{
		weapon_bg_3, weapon_bg_2,
		weapon_bg_2, weapon_bg_1,
		weapon_bg_2, weapon_bg_1,
		weapon_bg_3, weapon_bg_2,
	};

	const uint32_t image_weaponBgRight[8] =
	{
		weapon_bg_2, weapon_bg_3,
		weapon_bg_1, weapon_bg_2,
		weapon_bg_1, weapon_bg_2,
		weapon_bg_2, weapon_bg_3,
	};


	static const ProgressBarSkin *HEALTHBAR_DM;
	static const ProgressBarSkin *HEALTHBAR_BLUE;
	static const ProgressBarSkin *HEALTHBAR_RED;
	static const ProgressBarSkin *WEAPONBAR;
}



static void init()
{
	if (HEALTHBAR_DM == nullptr)
	{
		HEALTHBAR_DM = new ProgressBarSkin(COLOR_DM, COLOR_DM_BG, image_hpLeftDm, image_hpLeftBgDm, image_hpRightDm, image_hpRightBgDm);
		HEALTHBAR_BLUE = new ProgressBarSkin(COLOR_TEAM_BLUE, COLOR_TEAM_BLUE_BG, image_hpLeftBlue, image_hpLeftBgBlue, image_hpRightBlue, image_hpRightBgBlue);
		HEALTHBAR_RED = new ProgressBarSkin(COLOR_TEAM_RED, COLOR_TEAM_RED_BG, image_hpLeftRed, image_hpLeftBgRed, image_hpRightRed, image_hpRightBgRed);
		WEAPONBAR = new ProgressBarSkin(COLOR_WEAPON_BAR, COLOR_WEAPON_BAR_BG, image_weaponLeft, image_weaponBgLeft, image_weaponRight, image_weaponBgRight);
	}
}




const ProgressBarSkin *ProgressBarSkin::getHEALTHBAR_DM()
{
	init();
	return HEALTHBAR_DM;
}

const ProgressBarSkin *ProgressBarSkin::getHEALTHBAR_BLUE()
{
	init();
	return HEALTHBAR_BLUE;
}

const ProgressBarSkin *ProgressBarSkin::getHEALTHBAR_RED()
{
	init();
	return HEALTHBAR_RED;
}

const ProgressBarSkin *ProgressBarSkin::getWEAPONBAR()
{
	init();
	return WEAPONBAR;
}
