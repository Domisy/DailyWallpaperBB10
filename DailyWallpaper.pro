APP_NAME = DailyWallpaper

INCLUDEPATH += ../src ${QNX_TARGET}/usr/include/bb/platform

DEPENDPATH += ../src ${QNX_TARGET}/usr/include/bb/platform

CONFIG += qt warn_on cascades10
LIBS += -lbbdata 
LIBS += -lbbplatform
LIBS += -lbbsystem
QT+= network 
include(config.pri)