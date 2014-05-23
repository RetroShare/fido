!include("../Common/retroshare_plugin.pri"):error("Could not include file ../Common/retroshare_plugin.pri")

CONFIG += qt \
    uic \
    qrc \
    resources

SOURCES = \
    p3Fido.cpp \
    FidoPlugin.cpp \
    helpers.cpp

DEFINES += ZR_TESTNET

HEADERS = \
    p3Fido.h \
    FidoPlugin.h \
    helpers.h

LIBS += -lmimetic


TARGET = Fido
