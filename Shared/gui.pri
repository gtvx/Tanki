QT += gui widgets


SOURCES += \
	$$PWD/FrameConsole/frameconsole.cpp \
	$$PWD/FrameRegistration/frameregistration.cpp \
	$$PWD/debug.cpp \
	$$PWD/mainwindow/mainwindow.cpp \
	$$PWD/FrameBattle/framebattle.cpp \
	$$PWD/FrameLobby/framelobby.cpp \
	$$PWD/packages/sendpackage.cpp \
	$$PWD/graphics.cpp

HEADERS += \
	$$PWD/FrameConsole/frameconsole.h \
	$$PWD/FrameRegistration/frameregistration.h \
	$$PWD/debug.h \
	$$PWD/mainwindow/mainwindow.h \
	$$PWD/FrameBattle/framebattle.h \
	$$PWD/FrameLobby/framelobby.h \
	$$PWD/packages/sendpackage.h \
	$$PWD/graphics.h

FORMS += \
    $$PWD/FrameBattle/framebattle.ui \
    $$PWD/FrameConsole/frameconsole.ui \
	$$PWD/FrameLobby/framelobby.ui \
    $$PWD/FrameRegistration/frameregistration.ui \
	$$PWD/mainwindow/mainwindow.ui \
	$$PWD/packages/sendpackage.ui

