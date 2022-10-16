
TARGET = MT_GRAPHICS

CONFIG += CLASS_NORMAL
CONFIG += TANK_BOT

CONFIG += __BONUS_TYPE_1

include(../Shared/Shared.pri)
include(../Shared/graphics_gui.pri)
include(../Shared/graphics.pri)

HEADERS += \
    __bonus.h \
	__global.h \
    __tank_bot.h
