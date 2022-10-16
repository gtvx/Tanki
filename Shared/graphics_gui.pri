QT += gui widgets


SOURCES += \
    $$PWD/FrameGarage/graphics_framegarage.cpp \
	$$PWD/mainwindow/mainwindow.cpp \
	$$PWD/FrameBattle/graphics_framebattle.cpp \
	$$PWD/FrameLobby/framelobby.cpp \
	$$PWD/packages/sendpackage.cpp \
	$$PWD/FrameConsole/frameconsole.cpp \
	$$PWD/FrameRegistration/frameregistration.cpp \

HEADERS += \
    $$PWD/FrameGarage/graphics_framegarage.h \
	$$PWD/mainwindow/mainwindow.h \
	$$PWD/FrameBattle/graphics_framebattle.h \
	$$PWD/FrameLobby/framelobby.h \
	$$PWD/packages/sendpackage.h \
	$$PWD/FrameConsole/frameconsole.h \
	$$PWD/FrameRegistration/frameregistration.h \

FORMS += \
    $$PWD/FrameBattle/graphics_framebattle.ui \
	$$PWD/FrameGarage/graphics_framegarage.ui \
	$$PWD/FrameLobby/framelobby.ui \
	$$PWD/mainwindow/mainwindow.ui \
	$$PWD/packages/sendpackage.ui \
	$$PWD/FrameConsole/frameconsole.ui \
	$$PWD/FrameRegistration/frameregistration.ui \


