QT += core scxml network xml
QT -= gui

CONFIG += c++11

TARGET = QtScxmlTester
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

qtHaveModule(scxml-private) {
    QT += scxml-private
    DEFINES += USE_SCXML_TRIGGERED_TRANSITIONS
}

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += Monitor/

HEADERS += \
    ../../Include/scxmlexternmonitor2.h

VERSION = 2.0.0.2

