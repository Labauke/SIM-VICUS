# Project file for DummyImportPlugin

TARGET        = SVDummyImportPlugin
TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets

# this pri must be sourced from all our applications
include( ../Qt/plugin.pri )

LIBS += \
	-lVicus \
	-lNandrad \
	-lIBKMK \
	-lIBK \
	-lTiCPP

INCLUDEPATH  += \
	../../SIM-VICUS/src \
	../../SIM-VICUS/src/plugins \
	../../externals/Vicus/src \
	../../externals/IBK/src \
	../../externals/IBKMK/src \
	../../externals/TiCPP/src

HEADERS       = src/DummyImportPlugin.h
SOURCES       = src/DummyImportPlugin.cpp

DISTFILES += data/metadata.json

